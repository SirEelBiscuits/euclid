Maths = Maths or {
	Vector = CreateNewClass(),
	Matrix = CreateNewClass(),

	RotationMatrix = function(angle)
		local c = math.cos(math.rad(angle))
		local s = math.sin(math.rad(angle))
		return Maths.Matrix:new(c, -s, s, c)
	end,
}

function Maths.Vector:new(x, y, z)
	local newInst = {
		x = x or 0,
		y = y or 0,
		z = z or 0
	}
	setmetatable(newInst, self)
	return newInst
end

function Maths.Vector.Mult(left, right)
	if type(left) == "number" then
		left, right = right, left
	end
	return Maths.Vector:new(left.x * right, left.y * right, left.z * right)
end

function Maths.Vector.Div(left, right)
	return Maths.Vector:new(left.x / right, left.y / right, left.z / right)
end

function Maths.Vector.Add(left, right)
	return Maths.Vector:new(left.x + right.x, left.y + right.y, left.z + right.z)
end

function Maths.Vector.Sub(left, right)
	return Maths.Vector:new(left.x - right.x, left.y - right.y, left.z - right.z)
end

function Maths.Vector:LengthSquared()
	return self.x * self.x + self.y * self.y + self.z * self.z
end

function Maths.Matrix:new(a, b, c, d)
	local newInst = {
		a = a or 0,
		b = b or 0,
		c = c or 0,
		d = d or 0
	}
	setmetatable(newInst, self)
	return newInst
end

function Maths.Matrix.Mult(left, right)
	if type(left) == "number" then
		left, right = right, left
	end

	if type(right) == number then
		return Maths.Matrix:new(
			left.a * right, left.b * right,
			left.c * right, left.d * right
		)
	else
		local mt = getmetatable(right)
		if mt == Maths.Vector then
			return Maths.Vector:new(
				left.a * right.x + left.b * right.y,
				left.c * right.x + left.d * right.y,
				right.z
			)
		elseif mt == Maths.Matrix then
			return Maths.Matrix:new(
				left.a * right.a + left.b * right.x, left.a * right.b + left.b * right.d,
				left.a * right.x + left.c * right.d, left.a * right.b + left.d * right.d
			)
		else
			assert(false, "Bad multiplication!")
		end
	end
end

function Maths.Matrix.Div(left, right)
	return Maths.Matrix:new(
		left.a / right, left.b / right,
		left.c / right, left.d / right
	)
end


function Maths.Matrix.Add(left, right)
	return Maths.Matrix:new(
		left.a + right.a, left.b + right.b,
		left.c + right.x, left.d + right.d
	)
end

function Maths.Matrix.Sub(left, right)
	return Maths.Matrix:new(
		left.a - right.a, left.b - right.b,
		left.c - right.x, left.d - right.d
	)
end

Maths.Vector.__add = Maths.Vector.Add
Maths.Vector.__sub = Maths.Vector.Sub
Maths.Vector.__mul = Maths.Vector.Mult
Maths.Vector.__div = Maths.Vector.Div
Maths.Matrix.__add = Maths.Matrix.Add
Maths.Matrix.__sub = Maths.Matrix.Sub
Maths.Matrix.__mul = Maths.Matrix.Mult
Maths.Matrix.__div = Maths.Matrix.Div

print("luaclid maths loaded")
