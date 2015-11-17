Editor = Editor or { 
	State = {}, 
	TypingState = {}, 
	DefaultState = {},
	view = { eye = Maths.Vector:new(0, 0, 0), scale = 10, angle = 0 },
	Selection = {verts = {}, walls = {}, sectors = {}},
	Cursor = {},
}

function Game.Initialise()
	Textures = { text = Draw.GetTexture("Mecha.png") }


	Game.LoadControls(dofile("editor/editorcontrols.lua"))
	Game.LoadAndWatchFile("editor/DefaultState.lua")
	Game.LoadAndWatchFile("editor/TypingState.lua")
	Game.LoadAndWatchFile("editor/PreviewState.lua")
	Game.LoadAndWatchFile("editor/DragObjectState.lua")
	Game.LoadAndWatchFile("editor/MapUtility.lua")

	MapUtility.__index = MapUtility

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

function Editor:OpenMap(filename)
	print("trying to open " .. filename)
	self.curMapName = filename
	self.curMapData = dofile(filename)
	setmetatable(self.curMapData, MapUtility)

	self.curMapData:FixAllSectorWindings()

	self.curMap     = Game.OpenMap(Editor.curMapData)
	self.DebugText  = self.curMap:GetNumSectors()
	print("done")
end

function Editor:SaveMap(filename)
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

function Editor:ScreenFromWorld(vec)
	local relative = (vec - self.view.eye) * self.view.scale
	relative.y = -relative.y
	relative = Maths.RotationMatrix(self.view.angle or 0) * relative
	return Maths.Vector:new(
		relative.x + Draw.GetWidth() / 2,
		relative.y + Draw.GetHeight() / 2
	)
end

function Editor:WorldFromScreen(vec)
	local relative = Maths.Vector:new(vec.x - Draw.GetWidth() / 2, vec.y - Draw.GetHeight() / 2)
	relative = Maths.RotationMatrix(-self.view.angle or 0) * relative
	relative.y = -relative.y
	return (relative / self.view.scale) + self.view.eye
end

local function MakeVec(v)
	return Maths.Vector:new(v.x, v.y, v.z)
end

function Editor:DrawTopDownMap(colors)
	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local nWall = sec.walls[j % #sec.walls + 1]
			Draw.Line(
				self:ScreenFromWorld(MakeVec(self.curMapData.verts[wall.start])),
				self:ScreenFromWorld(MakeVec(self.curMapData.verts[nWall.start])),
				colors.walls
			)
		end
	end

	for i, vert in ipairs(self.curMapData.verts) do
		if self.Selection:IsVertSelected(i) then
			local v = self:ScreenFromWorld(MakeVec(vert))
			Draw.Rect(
				{
					x = v.x - 1,
					y = v.y - 1,
					w = 3,
					h = 3
				},
				colors.vertSelection
			)
		end
	end
end

function Editor:GetClosestVertIdx(vecInScreen, maxDist)
	local vec = self:WorldFromScreen(vecInScreen)
	local minDist = maxDist or 999999999999999
	local vertIdx = -1
	for i, vert in ipairs(self.curMapData.verts) do
		local diff2D = MakeVec(vert) - vec
		diff2D.z = 0
		local lenSq = diff2D:LengthSquared()
		if lenSq < minDist then
			minDist = lenSq
			vertIdx = i
		end
	end
	if vertIdx ~= -1 then
		return vertIdx, minDist
	end
end

function Editor.Selection:Clear()
	self.verts = {}
	self.walls = {}
	self.sectors = {}
end

function Editor.Selection:SelectVert(id)
	self.verts[id] = true
end

function Editor.Selection:DeselectVert(id)
	self.verts[id] = nil
end

function Editor.Selection:IsVertSelected(id)
	return self.verts[id] or false
end

function Editor.Selection:SelectWall(id)
	self.walls[id] = true
end

function Editor.Selection:DeselectWall(id)
	self.walls[id] = nil
end

function Editor.Selection:IsWallSelected(id)
	return self.walls[id] or false
end

function Editor.Selection:SelectSector(id)
	self.sectors[id] = true
end

function Editor.Selection:DeselectSector(id)
	self.sectors[id] = nil
end

function Editor.Selection:IsSectorSelected(id)
	return self.sectors[id] or false
end

