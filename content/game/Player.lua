Player = Player or CreateNewClass(Entity)

function Player:ctor()
	Entity.ctor(self)
	self.radius = 0.5
	self.height = 1.7

	self.eyeHeight = 1.58
	self.stepHeight = 0.2
	self.angle = 0
end

print("Player reloaded")
