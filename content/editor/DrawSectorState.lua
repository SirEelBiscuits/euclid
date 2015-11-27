Editor.DrawSectorState = Editor.DrawSectorState or {}
print("DrawSectorState reloaded")

function Editor.DrawSectorState:Enter()
	print("entering draw sector state")
	self.verts = {}
	Editor.State = self
end

function Editor.DrawSectorState:Leave(ToState)
	local newSector = {walls = {}}
	for i, v in ipairs(self.verts) do
		local vec = v
		if type(vec) ~= "number" then 
			table.insert(Editor.curMapData.verts, vec)
			vec = #Editor.curMapData.verts
		end
		table.insert(newSector.walls, {start = vec})
	end

	Editor.curMapData:SetSectorCentroid(newSector)
	table.insert(Editor.curMapData.sectors, newSector)
	Editor.curMapData:FixAllSectorWindings()

	-- 
	-- FIX PORTALS IN AND OUT ON SHARED WALLS
	--
	-- ALSO DO SOMETHING SENSIBLE WITH THE FLOOR AND CEILING HEIGHTS/TEXTURES
	
	local minFloor = nil
	local maxCeil  = nil
	
	for secIdx, sec in ipairs(Editor.curMapData.sectors) do
		for otherWallIdx, otherWall in ipairs(sec.walls) do
			local otherNextWall = sec.walls[otherWallIdx % #sec.walls + 1]
			for wallIdx, wall in ipairs(newSector.walls) do
				nextWall = newSector.walls[wallIdx % #newSector.walls + 1]
				if wall.start == otherNextWall.start and nextWall.start == otherWall.start then
					otherWall.portal = #Editor.curMapData.sectors
					wall.portal = secIdx
					if minFloor == nil or (sec.floorHeight or 0) < minFloor then
						minFloor = sec.floorHeight
					end
					if maxCeil == nil or (sec.ceilHeight or 2) > maxCeil then
						maxCeil = sec.ceilHeight
					end
				end
			end
		end
	end

	newSector.ceilHeight = maxCeil or 2
	newSector.floorHeight = minFloor or 0

	Editor.State = ToState
end

function Editor.DrawSectorState:IsVertPosValid(pos)
	if #self.verts < 3 then
		return true
	end

	table.insert(self.verts, pos)

	local allPos = true
	local allNeg = true

	for i in ipairs(self.verts) do
		local v1 = self.verts[i]
		if type(v1) == "number" then v1 = Editor.curMapData.verts[v1] end
		local v2 = self.verts[i % #self.verts + 1]
		if type(v2) == "number" then v2 = Editor.curMapData.verts[v2] end
		local v3 = self.verts[(i + 1) % #self.verts + 1]
		if type(v3) == "number" then v3 = Editor.curMapData.verts[v3] end

		local XP = Cross2D(v2 - v1, v3 - v1)
		allPos = allPos and XP >= 0
		allNeg = allNeg and XP <= 0
	end

	table.remove(self.verts)

	return allPos or allNeg
end

function Editor.DrawSectorState:Update(dt)
	local nearVert = Editor:GetClosestVertIdx(Editor.Cursor, 1/Editor.view.scale)
	local vert = nearVert or Editor.Cursor
	self.vertValid = self:IsVertPosValid(vert)

	if Game.Controls.DrawSectorPlaceVert.pressed then
		if self.vertValid then
			table.insert(self.verts, vert)
		end
	end

	if Game.Controls.DrawSectorUnplaceVert.pressed then
		if #self.verts > 0 then
			table.remove(self.verts)
		end
	end

	if Game.Controls.DrawSectorModeDone.pressed then
		self:Leave(Editor.DefaultState)
	end

	if Game.Controls.DrawSectorModeCancel.pressed then
		Editor.State = Editor.DefaultState
	end
end

function Editor.DrawSectorState:Render()
	Editor:DrawTopDownMap(Editor.Colors)
	local prevVec = nil
	for i, v in ipairs(self.verts) do
		local vec = v
		if type(vec) == "number" then vec = Editor.curMapData.verts[vec] end
		vec = Editor:ScreenFromWorld(vec)
		if prevVec ~= nil then
			Draw.Line({x = prevVec.x, y = prevVec.y}, {x = vec.x, y = vec.y}, Editor.Colors.vertDrawing)
		end
		Draw.Rect({ x = vec.x - 1, y = vec.y - 1, w = 3, h = 3}, Editor.Colors.vertDrawing)
		prevVec = vec
	end

	local ScreenCursor = Editor:ScreenFromWorld(Editor.Cursor)
	if self.vertValid then
		if prevVec ~= nil then
			Draw.Line({x = ScreenCursor.x, y = ScreenCursor.y}, {x = prevVec.x, y = prevVec.y}, {g = 255})
		end
		Draw.Rect({x = ScreenCursor.x - 1, y = ScreenCursor.y - 1, w = 3, h = 3}, {g = 255})
	else
		if prevVec ~= nil then
			Draw.Line({x = ScreenCursor.x, y = ScreenCursor.y}, {x = prevVec.x, y = prevVec.y}, {r = 255})
		end
		Draw.Rect({x = ScreenCursor.x - 1, y = ScreenCursor.y - 1, w = 3, h = 3}, {r = 255})
	end
end
