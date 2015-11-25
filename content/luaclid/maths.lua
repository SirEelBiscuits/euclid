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
	return Maths.Vector:new(left.x + right.x, left.y + right.y, (left.z or 0) + (right.z or 0))
end

function Maths.Vector.Sub(left, right)
	return Maths.Vector:new(left.x - right.x, left.y - right.y, (left.z or 0) - (right.z or 0))
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

function Cross2D(left, right)
	return left.x * right.y - right.x * left.y
end

function Dot(left, right)
	return left.x * right.x + left.y * right.y + left.z * right.z
end

function Maths.LineIntersect(line1, line2)
	local l1e = line1.e - line1.s -- line1 is line1.s to line1.s + l1e
	local l2e = line2.e - line2.s -- line2 is line2.s to line2.s + l2e
	local l2Rel = line2.s - line1.s

	local rXs = Cross2D(l1e, l2e)
	local t = Cross2D(l2Rel, l2e)
	local u = Cross2D(l2Rel, l1e)

	if rXs == 0 then
		return nil
		--[[
		if t == 0 then
			-- colinear
		else
			-- parallel
		end --]]--
	else
		t = t / rXs
		u = u / rXs
		local point = line1.s + t * l1e
		return (t >= -0.0000000000001 and t <= 1 and u >= -0.0000000000001 and u <= 1), line1.s + t * l1e
	end
end

function Maths.PointLineSegDistance(point, lineStart, lineEnd)
	local pointRel = point - lineStart
	local lineEndRel = lineEnd - lineStart
	local lenSq = Dot(lineEndRel, lineEndRel)
	local xProd = Cross2D(lineEndRel, pointRel)
	local dist = xProd * xProd / lenSq
	if Dot(pointRel, lineEndRel) > 0
		and Dot(pointRel, lineEndRel) < lenSq
	then
		return dist
	else
		return math.sqrt(math.min(
			pointRel:LengthSquared(),
			(point - lineEnd):LengthSquared()
		))
	end
end

print("luaclid maths loaded")
