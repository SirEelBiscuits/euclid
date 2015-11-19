MapUtility = MapUtility or {}

--[[
	This object is designed to be set as the metatable of Editor.curMapData and similar
	the idea being to give some access to things to delete and insert data correctly,
	and query map specific information
]]--

function MapUtility:SetUpMap(map)
	self.__index = self
	setmetatable(map, self)
	for i, v in ipairs(map.verts) do
		setmetatable(v, Maths.Vector)
	end
	map:SetCentroids()
end

-- Verts

function MapUtility:GetVert(id)
	return Maths.Vector:new(self.verts[id].x, self.verts[id].y)
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

-- Walls

function MapUtility:SplitWall(secID, wallID)
	local sec = self.sectors[secID]
	local wall = sec.walls[wallID]
	local wallEndID = sec.walls[wallID % #sec.walls + 1].start
	local wallEnd = self:GetVert(wallEndID)

	local newVertPos = (self:GetVert(wall.start) + wallEnd) / 2
	table.insert(self.verts, newVertPos)
	local newWall = { 
		start = #self.verts, 
		topTex = DeepCopy(wall.topTex),
		mainTex = DeepCopy(wall.mainTex),
		bottomTex = DeepCopy(wall.bottonTex),
		portal = wall.portal
	}
	table.insert(sec.walls, wallID+1, newWall)

	-- now we have to check the wall on the far side of the portal if there is one
	-- as it will need to be split also
	
	local otherSec = self.sectors[wall.portal]
	if otherSec ~= nil then
		for i, wall in ipairs(otherSec.walls) do
			local nWall = self:GetVert(wall.start)
			if wall.start == wallEndID then
				local newWall = {
					start = #self.verts,
					topTex = DeepCopy(nWall.topTex),
					mainTex = DeepCopy(nWall.mainTex),
					bottomTex = DeepCopy(nWall.bottomTex),
					portal = nWall.portal
				}
				table.insert(otherSec.walls, i+1, newWall)
				break
			end
		end
	end

	return #self.verts
end

-- Sectors

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

	local vert = walls[#walls].start
	for i = #walls, 2, -1 do
		walls[i].start = walls[i-1].start
	end
	walls[1].start = vert
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
			if wall.portal ~= nil then
				if wall.portal == id then
					wall.portal = nil
				end
				if wall.portal > id then
					wall.portal = wall.portal - 1
				end
			end
		end
	end
	table.remove(self.sectors, id)
end

function MapUtility:IsPointInSector(vec, secID)
	local sec = self.sectors[secID]
	for i, wall in ipairs(sec.walls) do 
		local start = self:GetVert(wall.start)
		local nextWall = sec.walls[i % #sec.walls + 1]
		local nextStart = self:GetVert(nextWall.start)

		if Cross2D(nextStart - start, vec - start) < 0 then
			return false
		end
	end
	return true
end

function MapUtility:SetSectorCentroid(sec)
	local acc = Maths.Vector:new(0,0)
	for j, wall in ipairs(sec.walls) do
		acc = acc + self:GetVert(wall.start)
	end
	acc = acc / #sec.walls
	sec.centroid = acc
end

function MapUtility:SetCentroids()
	for i, sec in ipairs(self.sectors) do
		self:SetSectorCentroid(sec)
	end
end

function MapUtility:SplitSector(SecID, wall1ID, wall2ID)
	if wall1ID == wall2ID then
		return
	end

	local oldSec = self.sectors[SecID]
	local newLeftSec = DeepCopy(oldSec)
	newLeftSec.walls = {}
	local leftSecID = #self.sectors + 1
	local newRightSec = DeepCopy(oldSec)
	newRightSec.walls = {}
	local rightSecID = #self.sectors + 2

	local left = wall1ID < wall2ID
	for i, wall in ipairs(oldSec.walls) do
		if left then
			table.insert(newLeftSec.walls, DeepCopy(wall))
		else
			table.insert(newRightSec.walls, DeepCopy(wall))
		end
		if i == wall1ID or i == wall2ID then
			left = not left
			if left then
				table.insert(newLeftSec.walls, DeepCopy(wall))
				newRightSec.walls[#newRightSec.walls] = {portal = leftSecID, start = wall.start}
			else
				table.insert(newRightSec.walls, DeepCopy(wall))
				newLeftSec.walls[#newLeftSec.walls] = {portal = rightSecID, start = wall.start}
			end
		end
	end

	-- now we stitch any portals leading in
	
	self:SetSectorCentroid(newLeftSec)
	self:SetSectorCentroid(newRightSec)
	
	for i, sec in ipairs(self.sectors) do
		for j, wall in ipairs(sec.walls) do
			if wall.portal == SecID then
				local wallCentre = (self:GetVert(wall.start) + self:GetVert( sec.walls[j % #sec.walls + 1].start )) / 2
				local toLeft = wallCentre - newLeftSec.centroid
				local toRight = wallCentre - newRightSec.centroid
				if Dot(toLeft, toLeft) < Dot(toRight, toRight) then
					wall.portal = leftSecID
				else
					wall.portal = rightSecID
				end
			end
		end
	end
	

	-- and add the new sectors to the list

	table.insert(self.sectors, newLeftSec)
	table.insert(self.sectors, newRightSec)

	self:DeleteSector(SecID)

end

print("Map Utility loaded")
