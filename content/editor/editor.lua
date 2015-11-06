Editor = Editor or { State = {}, TypingState = {}, DefaultState = {}}

function nullfunc()
end

function Game.Initialise()
	Textures = { text = Draw.GetTexture("Mecha.png") }


	Game.LoadControls(dofile("editor/editorcontrols.lua"))
	Game.LoadAndWatchFile("editor/DefaultState.lua")
	Game.LoadAndWatchFile("editor/TypingState.lua")
	Game.LoadAndWatchFile("editor/PreviewState.lua")

	Editor.State = Editor.DefaultState

	Game.quit = false

	print("initialised")
end

function Game.SaveState()
end
function Game.LoadState()
end

function Game.Quit()
	Game.quit = true;
end

function Game.Update(dt)
	Game.FPS = Game.FPS or 1
	local n = math.log(Game.FPS)
	Game.FPS = (n * Game.FPS + 1/dt) / (n+1)

	Editor.State:Update(dt)

	return not Game.quit
end

function Game.Render()
	Draw.Rect({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, {})

	Editor.State:Render()

	if Textures.text then
		local s = tostring(math.floor(Game.FPS))
		Draw.Text({x = Draw.GetWidth() - s:len() * 8 - 4, y = 4}, Textures.text, s)
	end
end

function openMap(filename)
	print("trying to open " .. filename)
	Editor.curMapName = filename
	Editor.curMapData = dofile(filename)
	Editor.curMap     = Game.OpenMap(Editor.curMapData)
	Editor.DebugText  = Editor.curMap:GetNumSectors()
	print("done")
end

function saveMap(filename)
	filename = filename or Editor.curMapName
	if filename == "" then
		filename = Editor.curMapName
	end
	print("trying to save " .. filename)
	Editor.curMapData = Game.StoreMap(Editor.curMap)
	local str = serialise(Editor.curMapData)
	local file = io.open(filename, "w")
	io.output(file)
	io.write(str)
	io.close(file)
	print("done")
end

