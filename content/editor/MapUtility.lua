MapUtility = MapUtility or {}

--[[
	This object is designed to be set as the metatable of Editor.curMapData and similar
	the idea being to give some access to things to delete and insert data correctly,
	and query map specific information
]]--

local function Cross2D(left, right)
	return left.x * right.y - right.x * left.y
end

function MapUtility:GetVert(id)
	return Maths.Vector:new(self.verts[id].x, self.verts[id].y)
end

function MapUtility:IsSectorConvex(id)
	local sec = self.sectors[id]
	for i, wall in ipairs(sec.walls) do
		local start = self:GetVert(wall.start)
		local nextWall = sec.walls[(i % #sec.walls) + 1]
		local nextStart = self:GetVert(nextWall.start)
		local nextWall2 = sec.walls[((i + 1) % #sec.walls) + 1]
		local nextStart2 = self:GetVert(nextWall2.start)

		if Cross2D(nextStart - start, nextStart2 - start) < 0 then
			return false
		end
	end
	return true
end

function MapUtility:ReverseSectorWinding(id)
	local walls = self.sectors[id].walls
	local count = (#walls) / 2
	for i = 1, count do
		local rIdx = #walls + 1 - i
		walls[i], walls[rIdx] = walls[rIdx], walls[i]
	end
end

function MapUtility:FixAllSectorWindings()
	for i, sec in ipairs(self.sectors) do
		if not self:IsSectorConvex(i) then
			self:ReverseSectorWinding(i)
			if not self:IsSectorConvex(i) then
				print( "WARNING: Sector " .. i .. " is nonconvex!")
			end
		end
	end
end

function MapUtility:DeleteSector(id)
	for i, sec in ipairs(self.sectors) do
		for j, wall in ipairs(sec.walls) do
			if wall.portal == id then
				wall.portal = nil
			end
		end
	end
	table.remove(self.sectors, id)
end

function MapUtility:DeleteVert(id)
	print("deleting vert " .. id)
	local SecUpdateList = {}
	for i, sec in ipairs(self.sectors) do
		local WallUpdateList = {}
		for j, wall in ipairs(sec.walls) do
			if wall.start == id then
				table.insert(WallUpdateList, j)
			end
			if wall.start >= id then
				wall.start = wall.start -1
			end
		end
		for j = #WallUpdateList, 1, -1 do
			table.remove(sec.walls, WallUpdateList[j])
		end

		if #sec.walls < 3 then
			table.insert(SecUpdateList, i)
		end
	end

	for j = #SecUpdateList, 1, -1 do
		self:DeleteSector(SecUpdateList[j])
	end

	table.remove(self.verts, id)
end

print("Map Utility loaded")
