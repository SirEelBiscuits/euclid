Editor.PreviewState = Editor.PreviewState or CreateNewClass("DragObjectState")


Editor.PreviewState.CommandList = {
	"PreviewRaiseCeiling",
	"PreviewLowerCeiling",
	"PreviewRaiseFloor",
	"PreviewLowerFloor",
}

print("PreviewState reloaded")

function Editor.PreviewState:OnEnter(editor)
	Game.ShowMouse(false)
	print("entering preview state")

	self.editor = editor


	editor.curMap = Game.OpenMap(editor.curMapData)

	self.radius = .5
	self.speed = 4
	self.secID = 1
	self.sector = editor.curMap:GetSector(self.secID - 1)
	editor.curMapData:SetCentroids()
	self.eye = editor.curMapData.sectors[self.secID].centroid
	self.eye.z = (editor.curMapData.sectors[self.secID].floorHeight or 0) + 1.65
	self.angle = 0
end

function Editor.PreviewState:CurSec()
	local editor = self.editor
	return editor.curMapData.sectors[self.secID]
end

function Editor.PreviewState:Update(dt)
	local editor = self.editor
	if Game.Controls.Quit.pressed then
		self:PopState()
		Game.ShowMouse(true)
	end

	for _, v in ipairs(self.CommandList) do
		local control = Game.Controls[v]
		if type(control) == "table" and control.pressed and editor.Commands[v] then
			editor.Commands[v](self)
		end
	end

	self.eye.z = self.eye.z + Game.Controls.RaiseCamera * dt
	self.angle = self.angle + Game.Controls.Turn

	local targetPos = self.eye + Maths.RotationMatrix(self.angle)
		* Maths.Vector:new(Game.Controls.PreviewRight, Game.Controls.PreviewForward, 0)
		* dt * self.speed

	self.secID, self.eye = editor.curMapData:SafeMove(self.secID, self.eye, targetPos)
	self.secID, self.eye =
		editor.curMapData:PopOutOfWalls(self.secID, self.eye, self.radius)
	self.eye.z = math.max(
		self:CurSec().floorHeight + 0.1,
		math.min(self:CurSec().ceilHeight - 0.1, self.eye.z)
	)
	self.sector = editor.curMap:GetSector(self.secID - 1)

	return true
end

function Editor.PreviewState:Render()
	local editor = self.editor
	local view = {eye = self.eye, angle = math.rad(self.angle), sector = self.sector}
	Draw.Map(view)
	Draw.TopDownMap(editor.curMap, view, {g = 255})
end
