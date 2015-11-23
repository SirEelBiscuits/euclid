Editor = Editor or { 
	State = {}, 
	TypingState = {}, 
	DefaultState = {},
	view = { eye = Maths.Vector:new(0, 0, 0), scale = 10, angle = 0 },
	Selection = {verts = {}, walls = {}, sectors = {}},
	Cursor = {},
	History = {level = 0, snapshots = {}},
}

Editor.Colors = {
	walls = {g = 128},
	vert = {g = 128},
	vertSelection = {b = 255},
	sectorSelection = {g = 255},
	sectorNonConvex = {r = 255},
	wallSelection = {b = 128},
	vertDrawing = {b = 255, g = 255},
	selectedTexture = {g = 255},
}

function GetEmptyMap()
	local ret =  {
		sectors = {},
		verts = {}
	}
	MapUtility:SetUpMap(ret)
	return ret
end

function Game.Initialise()
	Textures = { text = Draw.GetTexture("Mecha.png") }

	Editor.Controls = dofile("editor/editorcontrols.lua")
	Game.LoadControls(Editor.Controls)
	Game.LoadAndWatchFile("editor/DefaultState.lua")
	Game.LoadAndWatchFile("editor/TypingState.lua")
	Game.LoadAndWatchFile("editor/PreviewState.lua")
	Game.LoadAndWatchFile("editor/DragObjectState.lua")
	Game.LoadAndWatchFile("editor/DrawSectorState.lua")
	Game.LoadAndWatchFile("editor/TexturePickerState.lua")
	Game.LoadAndWatchFile("editor/MapUtility.lua")

	MapUtility.__index = MapUtility

	Editor.DefaultState:Enter()
	Editor.curMapData = GetEmptyMap()
	Editor.History:Clear()

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

	if false then
		if #Game.Input > 0 and not Game.Input[1].keyRepeat then
			print(Game.Input[1].key)
		end
	end

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
	MapUtility:SetUpMap(self.curMapData)

	self.curMapData:FixAllSectorWindings()

	self.curMap     = Game.OpenMap(Editor.curMapData)
	self.History:Clear()
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
	if self.curMapData == nil then
		return
	end

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

	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local nWall = sec.walls[j % #sec.walls + 1]
			if self.Selection:IsWallSelected(i, j) then
				Draw.Line(
					self:ScreenFromWorld(MakeVec(self.curMapData.verts[wall.start])),
					self:ScreenFromWorld(MakeVec(self.curMapData.verts[nWall.start])),
					colors.wallSelection
				)
			end
		end
	end

	for i, sec in ipairs(self.curMapData.sectors) do
		local v = self:ScreenFromWorld(sec.centroid)
		if self.curMapData:IsSectorConvex(i) == false then
			Draw.Rect(
				{
					x = v.x - 2,
					y = v.y - 2,
					w = 5,
					h = 5,
				},
				colors.sectorNonConvex
			)
		end
		if self.Selection:IsSectorSelected(i) then
			Draw.Rect(
				{
					x = v.x - 1,
					y = v.y - 1,
					w = 3,
					h = 3
				},
				colors.sectorSelection
			)
		end
	end

	for i, vert in ipairs(self.curMapData.verts) do
		local color
		if self.Selection:IsVertSelected(i) then
			color = colors.vertSelection
		else
			color = colors.vert
		end
		local v = self:ScreenFromWorld(MakeVec(vert))
		Draw.Rect(
			{
				x = v.x - 1,
				y = v.y - 1,
				w = 3,
				h = 3
			},
			color
		)
	end
end

function Editor:GetClosestVertIdx(vec, maxDist)
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

function Editor:GetClosestWallIdx(vec, maxDist)
	local minDist = maxDist or 9999999999999999
	local secIdx = -1
	local wallIdx = -1
	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local start = self.curMapData:GetVert(wall.start)
			local wallEnd = self.curMapData:GetVert(self.curMapData.sectors[i].walls[j % #self.curMapData.sectors[i].walls + 1].start) - start
			local wallLenSq = Dot(wallEnd, wallEnd)
			local vecRel = vec - start

			local crossProd = Cross2D(wallEnd, vecRel)
			local dist = crossProd * crossProd / wallLenSq

			if Dot(vecRel, wallEnd) > 0
				and Dot(vecRel, wallEnd) < wallLenSq
				and dist < minDist
			then
				minDist = dist
				secIdx = i
				wallIdx = j
			end
		end
	end

	if secIdx ~= -1 then
		return secIdx, wallIdx, minDist
	end
end

function Editor:GetControlsKey(Name)
	for i, c in ipairs(self.Controls) do
		if c.Name == Name then
			if c.ShiftPressed then
				return "shift+" .. c.Key
			else
				return c.Key
			end
		end
	end
end

function Editor:GetSelectionString()
	local verts = self.Selection:GetSelectedVerts()
	local walls = self.Selection:GetSelectedWalls()
	local sectors = self.Selection:GetSelectedSectors()
	

	if #sectors > 0 and #verts == 0 and #walls == 0 then
		local sect = self.curMapData.sectors[sectors[1]]

		local sec1 = self.curMapData.sectors[sectors[1]]

		local ceilH = sec1.ceilHeight or "multiple values"
		local floorH = sec1.floorHeight or "multiple values"
		local ceilTex = sec1.ceilTex ~= nil and sec1.ceilTex.tex or nil
		local floorTex = sec1.floorTex ~= nil and sec1.floorTex.tex or nil
		for i,s in ipairs(sectors) do
			local sec = self.curMapData.sectors[s]
			if sec.ceilHeight ~= ceilH then
				ceilH = "multiple values"
			end
			if sec.floorHeight ~= floorH then
				floorH = "multiple values"
			end
			if sec.ceilTex == nil or sec.ceilTex.tex ~= ceilTex then
				ceilTex = nil
			end
			if sec.floorTex == nil or sec.floorTex.tex ~= floorTex then
				floorTex = nil
			end
		end

		local workingString = ""
		if #sectors == 1 then
			workingString = "Sector id: " .. sectors[1] .. "\n"
		else
			workingString = #sectors .. " sectors selected\n"
		end
	
		workingString = workingString 
			.. "ceil height: " .. ceilH .. "[" .. (self:GetControlsKey("SetCeilHeight") or "???") .. "] to edit\n" 
			.. "floor height: " .. floorH .. "[" .. (self:GetControlsKey("SetFloorHeight") or "???") .. "] to edit\n" 
		local count = 3
		local ret = {}

		workingString = workingString .. "ceil Texture: [" .. (self:GetControlsKey("SetCeilTexture") or "???") .. "] to edit\n"
		count = count + 1
		if ceilTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = ceilTex })
			count = count + 4
		end

		workingString = workingString .. "floor Texture: [" .. (self:GetControlsKey("SetFloorTexture") or "???") .. "] to edit\n"
		count = count + 1
		if floorTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, {pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = floorTex})
			count = count + 4
		end

		return workingString, count, ret


	elseif #verts ~= 0 and #sectors == 0 and #walls == 0 then
		if #verts == 1 then
			local v = self.curMapData.verts[verts[1]]
			return "Selected vert: id = " .. verts[1] .. ", position: (" .. v.x .. ", " .. v.y .. ")", 1, {}
		else
			return #verts .. " verts selected", 1, {}
		end
	elseif #walls > 0 and #verts == 0 and #sectors == 0 then
		local wall1 = self.curMapData.sectors[walls[1].sec].walls[walls[1].wall]
		local topTex = wall1.topTex ~= nil and wall1.topTex.tex or nil
		if topTex == "" then topTex = nil end
		local mainTex = wall1.mainTex ~= nil and wall1.mainTex.tex or nil
		if mainTex == "" then mainTex = nil end
		local bottomTex = wall1.bottomTex ~= nil and wall1.bottomTex.tex or nil
		if bottomTex == "" then bottomTex = nil end

		for i, wall in ipairs(walls) do
			local wallref = self.curMapData.sectors[wall.sec].walls[wall.wall]
			if wallref.bottomTex == nil or wallref.bottomTex.tex ~= bottomTex then
				bottomTex = nil
			end
			if wallref.mainTex == nil or wallref.mainTex.tex ~= mainTex then
				mainTex = nil
			end
			if wallref.topTex == nil or wallref.topTex.tex ~= topTex then
				topTex = nil
			end
		end

		local workingString = ""

		if #walls == 1 then
			workingString = workingString .. "wall " .. walls[1].wall .. ", sector ".. walls[1].sec .. "\n"
		else
			workingString = workingString .. #walls .. " walls selected: \n"
		end

		local count = 1
		local ret = {}

		workingString = workingString .. "top Texture: [" .. (self:GetControlsKey("SetTopTexture") or "???") .. "] to edit\n"
		count = count + 1
		if topTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = topTex })
			count = count + 4
		end
		workingString = workingString .. "main Texture: [" .. (self:GetControlsKey("SetMainTexture") or "???") .. "] to edit\n"
		count = count + 1
		if mainTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = mainTex })
			count = count + 4
		end
		workingString = workingString .. "bottom Texture: [" .. (self:GetControlsKey("SetBottomTexture") or "???") .. "] to edit\n"
		count = count + 1
		if bottomTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = bottomTex })
			count = count + 4
		end

		return workingString, count, ret
	else
		local working = ""
		if #verts > 0 then 
			working = working .. #verts .. " vert" 
			if #verts > 1 then
				working = working .. "s"
			end
		end
		if #walls > 0 then
			if #working > 0 then
				if #sectors == 0 then
					working = working .. " and "
				else
					working = working .. ", "
				end
			end
			working = working .. #walls .. " wall"
			if #walls > 1 then
				working = working .. "s"
			end
		end
		if #sectors > 0 then
			if #working > 0 then
				working = working .. " and "
			end
			working = working .. #sectors .. " sector"
			if #sectors > 1 then
				working = working .. "s"
			end
		end
		return working, 1, {}
	end

	return "", 0, {}
end

function Editor.Selection:Clear(callback)
	self.verts = {}
	self.walls = {}
	self.sectors = {}
	self.OnSelectionChange = callback or function() end
	self.OnSelectionChange()
end

function Editor.Selection:SelectVert(id)
	self.verts[id] = true
	self.OnSelectionChange()
end

function Editor.Selection:DeselectVert(id)
	self.verts[id] = nil
	self.OnSelectionChange()
end

function Editor.Selection:IsVertSelected(id)
	return self.verts[id] or false
end

function Editor.Selection:GetSelectedVerts()
	local ret = {}
	for i in pairs(self.verts) do
		table.insert(ret, i)
	end
	return ret
end

function Editor.Selection:SelectWall(secID, wallID)
	self.walls[secID] = self.walls[secID] or {}
	self.walls[secID][wallID] = true
	self.OnSelectionChange()
end

function Editor.Selection:DeselectWall(secID, wallID)
	self.walls[secID][wallID] = nil
	self.OnSelectionChange()
end

function Editor.Selection:IsWallSelected(secID, wallID)
	return (self.walls[secID] or false) and self.walls[secID][wallID] or false
end

function Editor.Selection:GetSelectedWalls()
	local ret = {}
	for i, s in pairs(self.walls) do
		for j in pairs(s) do
			table.insert(ret, {sec = i, wall = j})
		end
	end
	return ret
end

function Editor.Selection:SelectSector(id)
	self.sectors[id] = true
	self.OnSelectionChange()
end

function Editor.Selection:DeselectSector(id)
	self.sectors[id] = nil
	self.OnSelectionChange()
end

function Editor.Selection:IsSectorSelected(id)
	return self.sectors[id] or false
end

function Editor.Selection:GetSelectedSectors()
	local ret = {}
	for i in pairs(self.sectors) do
		table.insert(ret, i)
	end
	return ret
end

function Editor.History:RegisterSnapshot()
	if self.level ~= 0 then
		local startIdx = #self.snapshots - self.level + 1
		print("Trimming history, " .. #self.snapshots .. " snapshots, trimming to level " .. self.level)
		for i = startIdx, #self.snapshots do
			self.snapshots[i] = nil
		end
	end
	self.level = 0

	Editor.curMapData = DeepCopy(Editor.curMapData)
	table.insert(self.snapshots, Editor.curMapData)
end

function Editor.History:Undo()
	self.level = self.level + 1
	if self.level >= #self.snapshots then
		self.level = self.level - 1
	end
	Editor.curMapData = self.snapshots[#self.snapshots - self.level]
end

function Editor.History:Redo()
	self.level = self.level - 1
	if self.level < 0 then self.level = 0 end
	Editor.curMapData = self.snapshots[#self.snapshots - self.level]
end

function Editor.History:Clear()
	self.level = 0
	self.snapshots = {}
	self:RegisterSnapshot()
end
