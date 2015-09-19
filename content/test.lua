Game = Game or {}

function Game.Initialise()
	print("Initialised")
end


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
	return true
end
