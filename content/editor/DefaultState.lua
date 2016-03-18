Editor.DefaultState = Editor.DefaultState or CreateNewClass("EditorDefaultState")
Editor.DefaultState.CommandList = {
	"reinit",
	"Quit",
	"OpenMap",
	"Save",
	"Undo",
	"Redo",
	"Copy",
	"Paste",
	"CopyProperties",
	"PasteProperties",
	"AddSelect",
	"ExclusiveSelect",
	"DeleteObject",
	"SplitWall",
	"JoinSectors",
	"EnterDrawSectorMode",
	"SetCeilHeight",
	"SetFloorHeight",
	"AdjustCeilHeight",
	"AdjustFloorHeight",
	"SetLightLevel",
	"SetCeilTexture",
	"SetFloorTexture",
	"SetTopTexture",
	"SetMainTexture",
	"SetBottomTexture",
	"ZoomIn",
	"ZoomOut",
	"SetStartSector",
	"SetGroup",
	"SelectGroup",
	"SetOutdoors",
	"SetIndoors",
	"SetOutdoorLevel",
	"CreateSprite",
	"EditThing",
	"Preview",
}

print("DefaultState reloaded")

local InfoString
local InfoStringHeight
local InfoStringTextures

function Editor.DefaultState:OnEnter()
	print("entering default state")
	self.OnSelectionChanged = function()
		InfoString, InfoStringHeight, InfoStringTextures
			= self.machine.owner:GetSelectionString()
		end

	self.machine.owner.Selection:Clear(self.OnSelectionChanged)
end

function Editor.DefaultState:OnExit()
end

function Editor.DefaultState:OnPushed()
end

function Editor.DefaultState:OnPopped()
	self.OnSelectionChanged()
end

function Editor.DefaultState.OnSelectionChanged(editor)
	InfoString, InfoStringHeight, InfoStringTextures
		= editor:GetSelectionString()
end

function Editor.DefaultState:Render()
	self.machine.owner:DrawTopDownMap(self.machine.owner.Colors)

	if Textures.text then
		local CursorString = tostring(self.machine.owner.Cursor.x) .. ", "
			.. tostring(self.machine.owner.Cursor.y)
		Draw.Text(
			{x = Draw.GetWidth() - 8 * #CursorString - 4, y = Draw.GetHeight() - 20},
			Textures.text,
			CursorString
		)
		local drawPos = Maths.Vector:new(4, Draw.GetHeight() - 16 * InfoStringHeight - 4)
		Draw.Text(drawPos, Textures.text, InfoString)
		for i, info in ipairs(InfoStringTextures) do
			local pos = drawPos + info.pos
			pos.w, pos.h = info.pos.w, info.pos.h
			Draw.RectTextured(pos, Draw.GetTexture(info.tex))
		end
	end
end

function Editor.DefaultState:Update(dt)
	local editor = self.machine.owner
	if Game.Controls.DragObject.isDown then
		self.machine.owner.History:RegisterSnapshot()
		editor:PushState(Editor.DragObjectState)
		return
	end

	if Game.Controls.DragCamera.isDown then
		editor:PushState(Editor.CameraDragState)
		return
	end

	if Game.Controls.ScaleObject.isDown then
		self.machine.owner.History:RegisterSnapshot()
		editor:PushState(Editor.ScaleSelectionState)
		return
	end

	if Game.Controls.RotateObject.isDown then
		self.machine.owner.History:RegisterSnapshot()
		editor:PushState(Editor.RotateSelectionState)
		return
	end

	if Game.Controls.CreateDragObject.isDown then
		self.machine.owner.History:RegisterSnapshot()
		Editor.Commands.CreateDragObject(self.machine.owner)
		editor:PushState(Editor.DragObjectState)
		return
	end

	for _, v in ipairs(self.CommandList) do
		local control = Game.Controls[v]
		if type(control) == "table" and control.pressed and Editor.Commands[v] then
			Editor.Commands[v](self.machine.owner)
		end
	end
end

