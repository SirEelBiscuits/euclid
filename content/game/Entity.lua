Entity = Entity or CreateNewClass()

function Entity:ctor()
	self.radius = self.radius or 0.5
	self.height = self.height or 1.8
	self.eyeHeight = self.eyeHeight or 0
	self.stepHeight = self.stepHeight or 0
	self.map = self.map
	self.sector = self.sector or (self.map and self.map.defaultSector) or 1
	self.position = self.position or Maths.Vector:new(0, 0, 0)
	self.velocity = self.velocity or Maths.Vector:new(0, 0, 0)
	self.sprite = self.sprite
end

function Entity:Update(dt)
	self.grounded = false
	self.velocity.z = self.velocity.z - 9.8  * dt -- meters per second squared

	local newSec, newPos = self.map:SafeMove(
		self.sector,
		self.position,
		self.position + self.velocity * dt,
		function(startSec, ToSec)
			local sec = self.map.sectors[ToSec]
			return
				sec.floorHeight < self.position.z + self.stepHeight
				and sec.ceilHeight > self.position.z + self.height
		end
	)
	self.sector, self.position = self.map:PopOutOfWalls(
		newSec,
		newPos,
		self.radius,
		function(startSec, ToSec)
			local sec = self.map.sectors[ToSec]
			return
				sec.floorHeight < self.position.z + self.stepHeight
				and sec.ceilHeight > self.position.z + self.height
		end
	)


	local cursec = self.map.sectors[self.sector]
	if self.position.z + self.height > cursec.ceilHeight then
		self.velocity.z = 0
		self.position.z = cursec.ceilHeight - self.height
	end
	if self.position.z < cursec.floorHeight then
		self.position.z = cursec.floorHeight
		self.velocity.z = 0
		self.grounded = true
	end
end

print("Entity reloaded")
