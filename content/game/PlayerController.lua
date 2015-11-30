PlayerController = PlayerController or CreateNewClass()
PlayerController.jumpImpulse = 4

function PlayerController:ctor()
end

function PlayerController:Update(dt)
	local entity = self.entity

	if Game.Controls.Jump.pressed and entity.grounded == true then
		entity.velocity.z = self.jumpImpulse
	end

	entity.angle = entity.angle + Game.Controls.Turn

	local zStore = entity.velocity.z
	entity.velocity = 
		Maths.RotationMatrix(entity.angle)
		* Maths.Vector:new(Game.Controls.Right, Game.Controls.Forward, 0)
		* 4 -- magic num?
	entity.velocity.z = zStore
end

print("Player Controller reloaded")
