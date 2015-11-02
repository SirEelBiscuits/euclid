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

inputKey = 0
Game.oldCalls = 0
Game.Input={}

tex = Draw.GetTexture("ceil.png")

inputState = {x = 0, y = 0, a = 0}
speed = 1
turnSpeed = 30

function Game.Update(dt)
	Game.timerunning = (Game.timerunning or 0) + dt
	Game.timesecs = (Game.timesecs or 0) + dt
	Game.calls = (Game.calls or 0) + 1

	if(#Game.Input > 0) then
		print(Game.Input[1].key)
		inputKey = Game.Input[1].key
	else
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
		angle = 135
		pos = {x = 0, y = 0.5, z = 1.8}
	end

	
	if #Game.Input > 0 and Game.Input[1].eventType == InputEventType.KeyDown then
		if Game.Input[1].key == 1073741893 then -- F12
			Game.OpenMap(dofile("testmap.lua"))
		end

		if Game.Input[1].key == 27 then -- Escape
			Game.quit = true;
		end
	end

	for i, input in ipairs(Game.Input) do
		if input.keyRepeat == false then
			if input.key == 119 then --W
				if input.eventType == InputEventType.KeyDown then
					inputState.y = inputState.y + 1
				else
					inputState.y = inputState.y - 1
				end
			end
			if input.key == 115 then --S
				if input.eventType == InputEventType.KeyDown then
					inputState.y = inputState.y - 1
				else
					inputState.y = inputState.y + 1
				end
			end
			if input.key == 97 then --A
				if input.eventType == InputEventType.KeyDown then
					inputState.x = inputState.x + 1
				else
					inputState.x = inputState.x - 1
				end
			end
			if input.key == 100 then --D
				if input.eventType == InputEventType.KeyDown then
					inputState.x = inputState.x - 1
				else
					inputState.x = inputState.x + 1
				end
			end
			if input.key == 113 then --Q
				if input.eventType == InputEventType.KeyDown then
					inputState.a = inputState.a - 1
				else
					inputState.a = inputState.a + 1
				end
			end
			if input.key == 101 then --E
				if input.eventType == InputEventType.KeyDown then
					inputState.a = inputState.a + 1
				else
					inputState.a = inputState.a - 1
				end
			end
		end
	end

	angle = angle + inputState.a * dt * turnSpeed
	pos.x = pos.x + (inputState.x * math.cos(math.rad(-angle)) + inputState.y * math.sin(math.rad(-angle))) * dt * speed 
	pos.y = pos.y + (inputState.x * math.sin(math.rad(-angle)) - inputState.y * math.cos(math.rad(-angle))) * dt * speed


	if angle > 360 then
		angle = angle - 360
	end

	Game.SetView(pos, angle, Game.GetMap():GetSector(0))

	-- keep running forever!
	return not Game.quit
end

function Game.SaveState()
	angleBackup = angle
	posBackup = {x = pos.x, y = pos.y, z = pos.z}
end

function Game.LoadState()
	angle = angleBackup
	pos = {x = posBackup.x, y = posBackup.y, z = posBackup.z}
end

function Game.PostRender()
	local w = 50
	local h = 50
	Draw.Rect({x = 1, y = 1, w = w - 1, h = h - 1}, {})
	local startpoint = {x = w / 2 - pos.x * w / 4, y = h / 2 + pos.y * h / 4}
	local endpoint = {}
	endpoint.x = startpoint.x - math.sin(math.rad(-angle)) * (w / 3)
	endpoint.y = startpoint.y - math.cos(math.rad(-angle)) * (h / 3)
	Draw.Line(startpoint, endpoint, {g = 255})
end

function Game.Quit()
	Game.quit = true;
end

