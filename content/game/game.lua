StateList = StateList or {}
Game.data = {}
function Game.Initialise()
	Game.LoadAndWatchFile("luaclid/MapUtility.lua")
	Game.LoadAndWatchFile("game/SFA.lua")
	Game.LoadAndWatchFile("game/Entity.lua")
	Game.LoadAndWatchFile("game/Player.lua")
	Game.LoadAndWatchFile("game/PlayerController.lua")
	Game.LoadAndWatchFile("game/Camera.lua")

	Game:EnterState(StateList.SFA)

	Game.quit = false
end

function Game.SaveState()
	dataBackup = DeepCopy(Game.data)
end

function Game.LoadState()
	Game.data = DeepCopy(dataBackup)
end

function Game:EnterState(NewState)
	if self.State and self.State.Leave then
		self.State:Leave()
	end

	NewState:Enter()
	self.State = NewState
end

function Game.Quit()
	Game.State:Quit()
end

function Game.Update(dt)
	return Game.State:Update(dt)
end

function Game.Render()
	Game.State:Render()
end

print("game reloaded")
