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
	end

	-- keep running forever!
	return not Game.quit
end

function Game.PostRender()
	local yval = Game.oldCalls / 10
	Draw.RectTextured({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, tex)
	Draw.Line({x = inputKey, y = yval }, {x = 100, y = 53}, {r = 255})
	Draw.Rect({x = 20, y = 50, w = 10, h = 10}, { r = 255, g = 255})
end

function Game.Quit()
	Game.quit = true;
end

