function Game.Initialise()
	curMap = Game.OpenMap(dofile("testmap.lua"))
	Game.LoadControls(dofile("controls.lua"))
	Game.ShowMouse(false)
	Game.quit = false
	Game.FPS = 0
	State = {
		inputState = {x = 0, y = 0, a = 0},
		speed = 1,
		angle = 0,
		pos = {x = 0, y = 0, z = 1.8},
	}
	SaveState = deepCopy(State)
	Textures = {
		text = Draw.GetTexture("Mecha.png")
	}
	print("Initialised")
end

function Game.SaveState()
	SaveState = deepCopy(State)
end

function Game.LoadState()
	State = deepCopy(SaveState or {})
end

function Game.Quit()
	Game.quit = true;
end

function Game.Update(dt)
	Game.FPS = (Game.FPS + 1/dt) / 2

	if Game.Controls.ReloadMap.pressed then
		curMap = Game.OpenMap(dofile("testmap.lua"))
		print("reloaded map")
	end

	if Game.Controls.Quit.pressed then
		Game.quit = true
	end

	State.angle = State.angle + Game.Controls.Turn
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

function Game.Render()
	Draw.Map({eye = State.pos, angle = math.rad(State.angle), sector = curMap:GetSector(0)})

	local s = tostring(math.floor(Game.FPS))
	Draw.Text({x = Draw.GetWidth() - s:len() * 8 - 2, y = 0}, Textures.text, s)
end

