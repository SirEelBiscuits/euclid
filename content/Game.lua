Game.LoadAndWatchFile("editor/Editor.lua")

StateList = StateList or {}
Game.data = Game.data or {
	UpdateList = NewWeakTable(),
	UpdateWhenPausedList = NewWeakTable(),
	KeepAliveList = {},
	DeletionList = NewWeakTable(),
}

StateList.QuitState = StateList.QuitState or CreateNewClass("QuitState")
function StateList.QuitState:Update()
	return false
end
function StateList.QuitState:Render()
end

NameObject(StateList, "State List")
NameObject(Game.data, "Game.data")
NameObject(Game.data.UpdateList, "Update List")
NameObject(Game.data.UpdateWhenPausedList, "Update when paused List")
NameObject(Game.data.KeepAliveList, "Keepalive List")

function Game.Initialise(startstate, extradata)
	print("extra data: ", type(extradata))
	Describe(extradata)
	print("end")
	Game.StateMachine = StateMachine:new()
	Game.StateMachine:EnterState(StateList.QuitState)

	Game.StateMachine:PushState(_G[startstate], nil, extradata)

	Game.Text = Draw.GetTexture("resources/Mecha.png")

	ConsoleVariables()
	Game.quit = false
end

function Game.RegisterObjectForUpdate(o)
	table.insert(Game.data.UpdateList, o)
end

function Game.UnregisterObjectForUpdate(o)
	local idx = 0
	for i, v in ipairs(Game.data.UpdateList) do
		if v == o then
			idx = i
			break
		end
	end
	if idx > 0 then
		table.remove(Game.data.UpdateList, idx)
	end
end

function Game.RegisterObjectForUpdateWhenPaused(o)
	table.insert(Game.data.UpdateWhenPausedList, o)
end

function Game.UnregisterObjectForUpdateWhenPaused(o)
	local idx = 0
	for i, v in ipairs(Game.data.UpdateWhenPausedList) do
		if v == o then
			idx = i
			break
		end
	end
	if idx > 0 then
		table.remove(Game.data.UpdateWhenPausedList, idx)
	end
end

function Game.GetUpdateList(paused)
	if paused then
		return Game.data.UpdateWhenPausedList
	else
		return Game.data.UpdateList
	end
end

function Game.RegisterObjectForKeepAlive(o)
	table.insert(Game.data.KeepAliveList, o)
end

function ConsoleVariables()
	player = Game.data.Player
	playerController = Game.data.PlayerController
	Map = Game.data.map
end

function Game.SaveState()
	dataBackup = DeepCopy(Game.data)
	Game.NotifyOfCopy()
end

function Game.LoadState()
	Game.data = DeepCopy(dataBackup)
	Game.NotifyOfCopy()
	ConsoleVariables()
end

function Game.NotifyOfCopy(base)
	base = base or Game.data
	for k, v in pairs(base) do
		if type(v) == "table" and type(v.postCopy) == "function" then
			v:postCopy()
		end
	end
end

function Game.Quit()
	Game.StateMachine:Quit()
end

function Game.Update(dt)
	local ret =  Game.StateMachine:Update(dt)
	if Game.ToState then
		local s = Game.ToState
		Game.ToState = nil
		Game.StateMachine:PushState(s)
	end
	return ret
end

function Game.Render()
	Game.StateMachine:Render()
end

print("game reloaded")
