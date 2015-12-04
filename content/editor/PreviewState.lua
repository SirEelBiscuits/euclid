Editor.PreviewState = Editor.PreviewState or {}
print("PreviewState reloaded")

function Editor.PreviewState:Enter()
	Game.ShowMouse(false)
	Editor.State = self
	print("entering preview state")


	Editor.curMap = Game.OpenMap(Editor.curMapData)

	self.radius = .5
	self.speed = 4
	self.secID = 1
	self.sector = Editor.curMap:GetSector(self.secID - 1)
	Editor.curMapData:SetCentroids()
	self.eye = Editor.curMapData.sectors[self.secID].centroid
	self.eye.z = (Editor.curMapData.sectors[self.secID].floorHeight or 0) + 1.65
	self.angle = 0
end

function Editor.PreviewState:Update(dt)
	if Game.Controls.Quit.pressed then
		Editor.DefaultState:Enter()
		Game.ShowMouse(true)
	end

	if Game.Controls.RaiseCeiling.pressed then
		self.sector:set_ceilHeight(self.sector:get_ceilHeight() + 0.1)
	end

	if Game.Controls.LowerCeiling.pressed then
		self.sector:set_ceilHeight(self.sector:get_ceilHeight() - 0.1)
	end

	if Game.Controls.RaiseFloor.pressed then
		self.sector:set_floorHeight(self.sector:get_floorHeight() + 0.1)
	end

	if Game.Controls.LowerFloor.pressed then
		self.sector:set_floorHeight(self.sector:get_floorHeight() - 0.1)
	end

	self.angle = self.angle + Game.Controls.Turn

	local targetPos = self.eye + Maths.RotationMatrix(self.angle) 
		* Maths.Vector:new(Game.Controls.PreviewRight, Game.Controls.PreviewForward, 0) * dt * self.speed

	self.secID, self.eye = Editor.curMapData:SafeMove(self.secID, self.eye, targetPos)
	self.secID, self.eye = Editor.curMapData:PopOutOfWalls(self.secID, self.eye, self.radius)
	self.sector = Editor.curMap:GetSector(self.secID - 1)
end

function Editor.PreviewState:Render()
	local view = {eye = self.eye, angle = math.rad(self.angle), sector = self.sector}
	Draw.Map(view)
	Draw.TopDownMap(Editor.curMap, view, {g = 255})
end
