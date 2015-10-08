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

function Game.Update(dt)
	Game.timerunning = (Game.timerunning or 0) + dt
	Game.timesecs = (Game.timesecs or 0) + dt
	Game.calls = (Game.calls or 0) + 1

	if(Game.timesecs > 1) then
		Describe(Game.wall)
		print("---")
		Describe(Game.wall:get_length())
		print("---")
		Describe(Game.wall:get_length():get_val())
		print("-------")
		--print(Game.wall:get_length())
		--print(Game.calls .. " calls per second")
		Game.calls = 0
		Game.timesecs = Game.timesecs - 1
	end

	-- keep running forever!
	return not Game.quit
end

function Game.Quit()
	Game.quit = true;
end

