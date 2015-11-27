StateList = StateList or {}
function Game.Initialise()
	Game.LoadAndWatchFile("luaclid/MapUtility.lua")
	Game.LoadAndWatchFile("game/SFA.lua")
	Game.LoadAndWatchFile("game/Entity.lua")
	Game.LoadAndWatchFile("game/Player.lua")

	Game:EnterState(StateList.SFA)

	Game.quit = false
end

function Game.SaveState()
	StateListBackup = DeepCopy(StateList)
	StateBackup = Game.State
end

function Game.LoadState()
	StateList = DeepCopy(StateListBackup)
	Game.State = StateBackup
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
