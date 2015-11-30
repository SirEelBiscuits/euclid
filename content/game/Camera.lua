Camera = Camera or CreateNewClass()

function Camera:ctor()
	-- self.entity - an entity to treat as a viewpoint
end

function Camera:GetView(mapRenderableLookup)
	local entity = self.entity
	return {
		eye = entity.position + Maths.Vector:new(0, 0, entity.eyeHeight or 1.5),
		sector = entity.map.renderable:GetSector(entity.sector - 1),
		angle = math.rad(entity.angle)
	}
end

print("Camera reloaded")
