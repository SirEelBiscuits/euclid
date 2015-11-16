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

function Editor:openMap(filename)
	print("trying to open " .. filename)
	self.curMapName = filename
	self.curMapData = dofile(filename)
	self.curMap     = Game.OpenMap(Editor.curMapData)
	self.DebugText  = self.curMap:GetNumSectors()
	print("done")
end

function Editor:saveMap(filename)
	if filename ~= "" then
		self.curMapName = filename or self.curMapName
	end
	print("trying to save " .. filename)
	local str = serialise(Editor.curMapData)
	local file = io.open(filename, "w")
	io.output(file)
	io.write(str)
	io.close(file)
	print("done")
end

function Editor.ScreenFromView(view, vec)
	local relative = (vec - view.eye) * view.scale
	relative.y = -relative.y
	relative = Maths.RotationMatrix(view.angle or 0) * relative
	return Maths.Vector:new(
		relative.x + Draw.GetWidth() / 2,
		relative.y + Draw.GetHeight() / 2
	)
end

function Editor.ViewFromScreen(view, vec)
	local relative = Maths.Vector:new(vec.x - Draw.GetWidth() / 2, vec.y - Draw.GetHeight() / 2)
	relative = Maths.RotationMatrix(-view.angle or 0) * relative
	relative.y = -relative.y
	return (relative / view.scale) + view.eye
end

local function MakeVec(v)
	return Maths.Vector:new(v.x, v.y, v.z)
end

function Editor:DrawTopDownMap(view, colors)
	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local nWall = sec.walls[j % #sec.walls + 1]
			Draw.Line(
				self.ScreenFromView(view, MakeVec(self.curMapData.verts[wall.start])),
				self.ScreenFromView(view, MakeVec(self.curMapData.verts[nWall.start])),
				colors.walls
			)
		end
	end
end

