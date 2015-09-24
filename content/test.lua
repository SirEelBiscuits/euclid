Game = Game or {}

function Game.Initialise()
	print("Initialised")
	local map = dofile("testmap.lua")

	local k,v
	for k,v in pairs(Game) do
		print(k, "=", v)
	end
	print("----")
	local k,v
	for k,v in pairs(Game.TestClass) do
		print(k, "=", v)
	end

	Game.testS:set_x(3)
	Game.testS:method()
end

Game.quit = false

function Game.Update(dt)
	Game.timerunning = (Game.timerunning or 0) + dt
	Game.timesecs = (Game.timesecs or 0) + dt
	Game.calls = (Game.calls or 0) + 1

	if(Game.timesecs > 1) then
		print(Game.calls .. " calls per second")
		Game.calls = 0
		Game.timesecs = Game.timesecs - 1
	end

	-- keep running forever!
	return not Game.quit
end

function Game.Quit()
	Game.quit = true;
end
