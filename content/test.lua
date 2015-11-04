function Game.Initialise()
	print("Initialised")
	local map = dofile("testmap.lua")
end

function Describe(t)
	print(type(t))
	if type(t) == "table" then
		local k,v
		for k,v in pairs(t) do
			print(k, "=", v)
		end

		local mt = getmetatable(t)
		if mt ~= nil then
			Describe(mt)
		end
	else
		print(t)
	end
end

Game.quit = false

reloaded = true

Game.oldCalls = 0
Game.Input={}

tex = Draw.GetTexture("ceil.png")
text = Draw.GetTexture("Mecha.png")

State = {}
State.inputState = {x = 0, y = 0, a = 0}
State.speed = 1
State.turnSpeed = 30
State.angle = 0
State.pos = {x = 0, y = 0, z = 1.8}

SaveState = deepCopy(State)

function Game.Update(dt)
	Game.timerunning = (Game.timerunning or 0) + dt
	Game.timesecs = (Game.timesecs or 0) + dt
	Game.calls = (Game.calls or 0) + 1

	if(#Game.Input > 0) then
		--print(Game.Input[1].key)
	end

	if(Game.timesecs > 1) then
		print(Game.calls .. " calls per second")
		Game.oldCalls = Game.calls
		Game.calls = 0
		Game.timesecs = Game.timesecs - 1
	end

	if(reloaded) then
		reloaded = false
		Game.OpenMap(dofile("testmap.lua"))
		Describe(Game.GetMap().GetSector)

		Game.LoadControls(dofile("controls.lua"))

		--text = Game.GetTexture("Mecha.bmp")
	end
	
	if Game.Controls.ReloadMap.pressed then
		Game.OpenMap(dofile("testmap.lua"))
		print("reloaded map")
	end

	if Game.Controls.Quit.pressed then
		Game.quit = true
	end

	State.angle = State.angle + Game.Controls.Turn * dt * State.turnSpeed
	State.pos.x = State.pos.x + 
		(Game.Controls.Right * math.cos(math.rad(State.angle)) - Game.Controls.Forward * math.sin(math.rad(State.angle))) * dt * State.speed 
	State.pos.y = State.pos.y + 
		(Game.Controls.Right * math.sin(math.rad(State.angle)) + Game.Controls.Forward * math.cos(math.rad(State.angle))) * dt * State.speed


	if State.angle > 360 then
		State.angle = State.angle - 360
	end

	Game.SetView(State.pos, State.angle, Game.GetMap():GetSector(0))

	-- keep running forever!
	return not Game.quit
end

function Game.SaveState()
	SaveState = deepCopy(State)
end

function Game.LoadState()
	State = deepCopy(SaveState or {})
end

function Game.PostRender()
	local w = 50
	local h = 50
	Draw.Rect({x = 1, y = 1, w = w - 1, h = h - 1}, {})
	local startpoint = {x = w / 2 - State.pos.x * w / 4, y = h / 2 + State.pos.y * h / 4}
	local endpoint = {}
	endpoint.x = startpoint.x + math.sin(math.rad(State.angle)) * (w / 3)
	endpoint.y = startpoint.y + math.cos(math.rad(State.angle)) * (h / 3)
	Draw.Line(startpoint, endpoint, {g = 255})

	Draw.Text({x = 0, y = 0}, text, "Test text for the win\nwith newlines!")
end

function Game.Quit()
	Game.quit = true;
end

