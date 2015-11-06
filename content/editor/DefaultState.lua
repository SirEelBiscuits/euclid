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
				openMap(filename)
				Editor.DefaultState:Enter()
			end
		)
	end

	if Game.Controls.Preview.pressed then
		Editor.PreviewState:Enter()
	end

	if Game.Controls.Save.pressed then
		print("saving")
		Editor.curMapData = Game.StoreMap(Editor.curMap)
		Editor.TypingState:Enter(
		function (filename)
			saveMap(filename)

			Editor.State = Editor.DefaultState
		end
		)
	end
end

function Editor.DefaultState.Render()
	if Editor.curMap ~= nil then
		Draw.TopDownMap(Editor.curMap, {eye = {x = 0, y = 0}, angle = 0, sector = Editor.curMap:GetSector(0)}, {g = 255})
	end

	if Textures.text then
		Draw.Text({x = 0, y = 0}, Textures.text, Editor.DebugText)
	end
end

