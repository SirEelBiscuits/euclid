Editor.DefaultState = Editor.DefaultState or {}
print("DefaultState reloaded")

function Editor.DefaultState:Enter()
	print("entering default state")
	Editor.State = self
end

function Editor.DefaultState:Update(dt)
	if Game.Controls.reinit.pressed then
		Game.Initialise()
	end

	if Game.Controls.Quit.pressed then
		Game.quit = true
	end

	if Game.Controls.OpenMap.pressed then
		Editor.TypingState:Enter(
			function(filename)
				Editor:openMap(filename)
				Editor.DefaultState:Enter()
			end
		)
	end

	if Game.Controls.Preview.pressed then
		Editor.PreviewState:Enter()
	end

	if Game.Controls.Save.pressed then
		print("saving")
		Editor.TypingState:Enter(
		function (filename)
			Editor:saveMap(filename)

			Editor.DefaultStatea:Enter()
		end
		)
	end
end

function Editor.DefaultState.Render()
	if Editor.curMap ~= nil then
		Editor:DrawTopDownMap(
			{ eye = Maths.Vector:new(0, 0), angle = 0, scale = 10 },
			{ walls = {g = 128} }
		)
	end

	if Textures.text then
		Draw.Text({x = 0, y = 0}, Textures.text, Editor.DebugText)
	end
end

