Player = Player or CreateNewClass(Entity)

function Player:new(map, sector, position)
	local newInst = Entity:new(0.5, 1.65, nil, map, sector, position)

	newInst.eyeHeight = 1.5
	newInst.angle = 0
	setmetatable(newInst, self)
	return newInst
end
