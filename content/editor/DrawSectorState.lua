Editor.DrawSectorState = Editor.DrawSectorState or
	CreateNewClass("DrawSectorState")
print("DrawSectorState reloaded")

function Editor.DrawSectorState:OnEnter()
	print("entering draw sector state")
	self.verts = {}
end

function Editor.DrawSectorState:OnExit()
	if self.exitIsCancel or #self.verts < 3 then return end

	local editor = self.machine.owner

	local newSector = {walls = {}}
	for i, v in ipairs(self.verts) do
		local vec = v
		if type(vec) ~= "number" then
			table.insert(editor.curMapData.verts, vec)
			vec = #editor.curMapData.verts
		end
		table.insert(newSector.walls, {start = vec})
	end

	editor.curMapData:SetSectorCentroid(newSector)
	table.insert(editor.curMapData.sectors, newSector)
	editor.curMapData:FixAllSectorWindings()

	--
	-- FIX PORTALS IN AND OUT ON SHARED WALLS
	--
	-- ALSO DO SOMETHING SENSIBLE WITH THE FLOOR AND CEILING HEIGHTS/TEXTURES

	local minFloor = nil
	local maxCeil  = nil

	for secIdx, sec in ipairs(editor.curMapData.sectors) do
		for otherWallIdx, otherWall in ipairs(sec.walls) do
			local otherNextWall = sec.walls[otherWallIdx % #sec.walls + 1]
			for wallIdx, wall in ipairs(newSector.walls) do
				nextWall = newSector.walls[wallIdx % #newSector.walls + 1]
				if wall.start == otherNextWall.start and nextWall.start == otherWall.start then
					otherWall.portal = #editor.curMapData.sectors
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
end

function Editor.DrawSectorState:IsVertPosValid(pos)
	if #self.verts < 3 then
		return true
	end

	local editor = self.machine.owner

	table.insert(self.verts, pos)

	local allPos = true
	local allNeg = true

	for i in ipairs(self.verts) do
		local v1 = self.verts[i]
		if type(v1) == "number" then v1 = editor.curMapData.verts[v1] end
		local v2 = self.verts[i % #self.verts + 1]
		if type(v2) == "number" then v2 = editor.curMapData.verts[v2] end
		local v3 = self.verts[(i + 1) % #self.verts + 1]
		if type(v3) == "number" then v3 = editor.curMapData.verts[v3] end

		local XP = Cross2D(v2 - v1, v3 - v1)
		allPos = allPos and XP >= 0
		allNeg = allNeg and XP <= 0
	end

	table.remove(self.verts)

	return allPos or allNeg
end

function Editor.DrawSectorState:Update(dt)
	local editor = self.machine.owner
	local nearVert = editor:GetClosestVertIdx(editor.Cursor,
		editor.selectionRange/editor.view.scale)
	local vert = nearVert or editor.Cursor
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
		self:PopState()
		return
	end

	if Game.Controls.DrawSectorModeCancel.pressed then
		self.exitIsCancel = true
		self:PopState()
		return
	end
end

function Editor.DrawSectorState:Render()
	local editor = self.machine.owner
	editor:DrawTopDownMap(Editor.Colors)
	local prevVec = nil
	for i, v in ipairs(self.verts) do
		local vec = v
		if type(vec) == "number" then vec = editor.curMapData.verts[vec] end
		vec = editor:ScreenFromWorld(vec)
		if prevVec ~= nil then
			Draw.Line({x = prevVec.x, y = prevVec.y}, {x = vec.x, y = vec.y}, Editor.Colors.vertDrawing)
		end
		Draw.Rect({ x = vec.x - 1, y = vec.y - 1, w = 3, h = 3}, Editor.Colors.vertDrawing)
		prevVec = vec
	end

	local ScreenCursor = editor:ScreenFromWorld(editor.Cursor)
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
