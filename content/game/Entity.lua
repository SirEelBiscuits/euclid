Entity = Entity or CreateNewClass()

function Entity:new(radius, height, sprite, map, sector, position)
	local NewInst = {
		radius = radius or 0.5,
		height = height or 1.8,
		map = map,
		sector = sector or (map and map.defaultSector) or 1,
		position = position or Maths.Vector:new(0, 0, 0),
		velocity = velocity or Maths.Vector:new(0, 0, 0),
		sprite = sprite,
	}

	setmetatable(NewInst, self)
	return NewInst
end

function Entity:Update(dt)
	local newSec, newPos = self.map:SafeMove(self.sector, self.position, self.position + self.velocity * dt)
	self.sector, self.position = self.map:PopOutOfWalls(newSec, newPos, self.radius)
end
