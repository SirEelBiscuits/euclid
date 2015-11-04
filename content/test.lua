function Game.Initialise()
	curMap = Game.OpenMap(dofile("testmap.lua"))
	print("Initialised")
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

FPS = 0


function Game.Update(dt)

	FPS = (FPS + 1/dt) / 2

	if(#Game.Input > 0) then
		--print(Game.Input[1].key)
	end

	if(reloaded) then
		reloaded = false

		Game.LoadControls(dofile("controls.lua"))

		--text = Game.GetTexture("Mecha.bmp")
	end
	
	if Game.Controls.ReloadMap.pressed then
		curMap = Game.OpenMap(dofile("testmap.lua"))
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

	-- keep running forever!
	return not Game.quit
end

function Game.SaveState()
	SaveState = deepCopy(State)
end

function Game.LoadState()
	State = deepCopy(SaveState or {})
end

function Game.Render()
	Draw.Map({eye = State.pos, angle = math.rad(State.angle), sector = curMap:GetSector(0)})

	local w = 50
	local h = 50
	Draw.Rect({x = 1, y = 1, w = w - 1, h = h - 1}, {})
	local startpoint = {x = w / 2 - State.pos.x * w / 4, y = h / 2 + State.pos.y * h / 4}
	local endpoint = {}
	endpoint.x = startpoint.x + math.sin(math.rad(State.angle)) * (w / 3)
	endpoint.y = startpoint.y + math.cos(math.rad(State.angle)) * (h / 3)
	Draw.Line(startpoint, endpoint, {g = 255})

	local s = tostring(math.floor(FPS))
	Draw.Text({x = Draw.GetWidth() - s:len() * 8 - 2, y = 0}, text, s )
end

function Game.Quit()
	Game.quit = true;
end

