Editor.DefaultState = Editor.DefaultState or {
	CommandList = {
		"reinit",
		"Quit",
		"OpenMap",
		"Preview",
		"Save",
		"Undo",
		"Redo",
		"AddSelect",
		"ExclusiveSelect",
		"DeleteObject",
		"SplitWall",
		"JoinSectors",
		"EnterDrawSectorMode",
		"SetCeilHeight",
		"SetFloorHeight",
		"SetLightLevel",
		"SetCeilTexture",
		"SetFloorTexture",
		"SetTopTexture",
		"SetMainTexture",
		"SetBottomTexture"
	}
}

print("DefaultState reloaded")

function Editor.DefaultState:Enter(skipClear)
	skipClear = skipClear or false
	print("entering default state")
	if not skipClear then
		Editor.Selection:Clear(self.OnSelectionChanged)
	else
		self.OnSelectionChanged()
	end
	Editor.State = self
end

function Editor.DefaultState.OnSelectionChanged()
	Editor.DefaultState.InfoString, Editor.DefaultState.InfoStringHeight, Editor.DefaultState.InfoStringTextures = Editor:GetSelectionString()
end

function Editor.DefaultState:Render()
	Editor:DrawTopDownMap(Editor.Colors)

	if Textures.text then
		local CursorString = tostring(Editor.Cursor.x) .. ", " .. tostring(Editor.Cursor.y)
		Draw.Text({x = Draw.GetWidth() - 8 * #CursorString - 4, y = Draw.GetHeight() - 20}, Textures.text, CursorString)
		self:SelectionInfo()
	end
end

function Editor.DefaultState:SelectionInfo()
	local drawPos = Maths.Vector:new(4, Draw.GetHeight() - 16 * self.InfoStringHeight - 4)
	Draw.Text(drawPos, Textures.text, self.InfoString)
	for i, info in ipairs(self.InfoStringTextures) do
		local pos = drawPos + info.pos
		pos.w, pos.h = info.pos.w, info.pos.h
		Draw.RectTextured(pos, Draw.GetTexture(info.tex))
	end
end

function Editor.DefaultState:Update(dt)
	if Game.Controls.DragObject.isDown then
		Editor.History:RegisterSnapshot()
		Editor.DragObjectState:Enter()
	end

	if Game.Controls.DragCamera.isDown then
		Editor.CameraDragState:Enter()
	end

	for _, v in ipairs(self.CommandList) do
		local control = Game.Controls[v]
		if type(control) == "table" and control.pressed and Editor.Commands[v] then
			Editor.Commands[v]()
		end
	end
end

