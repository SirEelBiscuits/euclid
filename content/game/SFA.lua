StateList.SFA = StateList.SFA or {
	defaultmapfile = "game/demomap.lua",
	UpdateList = {},
	maps = {},
}

function StateList.SFA:Enter()
	print("Entered main SFA state")
	Game.LoadControls(dofile("game/controls.lua"))

	Game.ShowMouse(false)

	Game.data.map = dofile(self.defaultmapfile)
	MapUtility:SetUpMap(Game.data.map)
	Game.data.map.renderable = Game.OpenMap(Game.data.map)

	Game.data.UpdateList = {}
	Game.data.UpdateList.Player = Player:new({map = Game.data.map})
	Game.data.UpdateList.PlayerController = 
		PlayerController:new({entity = Game.data.UpdateList.Player})

	Game.data.Camera = Camera:new({entity = Game.data.UpdateList.Player})
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

	for k, updatable in pairs(Game.data.UpdateList) do
		updatable:Update(dt)
	end

	return not Game.quit
end

function StateList.SFA:Render(dt)
	Draw.Map(Game.data.Camera:GetView(self.maps))
end

print("SFA.lua reloaded")
