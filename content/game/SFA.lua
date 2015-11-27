StateList.SFA = StateList.SFA or {
	defaultmapfile = "game/demomap.lua",
	EntityList = {}
}

function StateList.SFA:Enter()
	Game.LoadControls(dofile("game/controls.lua"))

	Game.ShowMouse(false)

	self.map = dofile(self.defaultmapfile)
	MapUtility:SetUpMap(self.map)
	self.mapRenderable = Game.OpenMap(self.map)

	self.Player = Player:new(self.map)
	table.insert(self.EntityList, self.Player)
end

function StateList.SFA:Leave()

end

function StateList.SFA:Quit()
	Game.quit = true
end

function StateList.SFA:Update(dt)
	if Game.Controls.Quit.pressed then
		Game.quit = true
	end

	self.Player.angle = self.Player.angle + Game.Controls.Turn

	self.Player.velocity = 
		Maths.RotationMatrix(self.Player.angle)
		* Maths.Vector:new(Game.Controls.Right, Game.Controls.Forward, 0)
		* 4

	for i, entity in ipairs(self.EntityList) do
		entity:Update(dt)
	end

	return not Game.quit
end

function StateList.SFA:Render(dt)
	Draw.Map({
		eye    = self.Player.position + Maths.Vector:new(0, 0, self.Player.eyeHeight),
		sector = self.mapRenderable:GetSector(self.Player.sector - 1),
		angle  = math.rad(self.Player.angle)
	})
end

