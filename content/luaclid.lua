function MakeEnum(table, name)
	return setmetatable({}, {
		__index = table,
		__newindex = function()
			if name then
				error("Attempted to modify" .. name .. " enum")
			else
				error("Attempted to modify enum")
			end
		end,
		__metatable = false
	})
end

function CreateNewClass(baseClass)
	local Class = {}
	Class.__index = Class

	function Class:new(o)
		local NewInst = o or {}
		setmetatable(NewInst, Class)
		if(NewInst.ctor) then
			NewInst:ctor()
		end
		return NewInst
	end

	if baseClass then
		setmetatable(Class, {__index = baseClass})
	end
	return Class
end

function CreateNativeClass()
	local Class = CreateNewClass()
	Class.fromData = function(self, data)
		return self:new({_data = data})
	end
	return Class
end

-- This MUST match the enum in Events.h
InputEventType = MakeEnum({
	None = 0,
	KeyDown = 1,
	KeyUp = 2,
	MouseDown = 3,
	MouseUp = 4,
	MouseMove = 5,
	TextInput = 6,
	},
	"InputEventType"
)

local function char(c)
	return ("\\%03d"):format(c:byte())
end

local function isArray(t)
	local i = 0
	for _ in pairs(t) do
		i = i+1
		if t[i] == nil then
			return false
		end
	end
	return true
end

function DeepCopy(obj, lookup)
	lookup = lookup or {}
	local objType = type(obj)
	local copy
	if objType == "table" then
		if lookup[obj] ~= nil then
			return lookup[obj]
		end

		copy = {}
		for k,v in pairs(obj) do
			copy[k] = DeepCopy(v, lookup)
		end
		setmetatable(copy, getmetatable(obj))
		lookup[obj] = copy
	else
		copy = obj
	end
	return copy
end

local function serializeInner (o, indent)
  local ret = ""
  indent = indent or ""
  if type(o) == "number" then
    return tostring(o)

	elseif type(o) == "boolean" then
    return tostring(o)

  elseif type(o) == "string" then
    return ('"%s"'):format(o:gsub("[^ !#-~]", char))

  elseif type(o) == "table" then
		local isArr = isArray(o)
    ret = ret .. ("{\n")
    for k,v in pairs(o) do
			if type(k) == "string" then
				ret = ret .. indent .. k .. " = "
			elseif isArr then
				ret = ret .. indent
			else
	      ret = ret .. indent .. "[" .. serializeInner(k) .. "]" .. " = "
			end
      ret = ret .. serializeInner(v, "  " .. indent)
      ret = ret .. ",\n"
    end
    ret = ret .. string.sub(indent,3) .. "}"

  elseif type(o) == "nil" then
    ret = ret .. "nil"

  else
    error("cannot serialize a " .. type(o))
  end
  return ret
end

-- returns a string which will be a lua statement returning data of that value
-- if it can serialise it at all, that is
function serialise(o)
  return "return " .. serializeInner(o, "  ") .. "\n"
end

function Describe(t, indent)
	indent = indent or ""
	print(indent..type(t)..":")
	if type(t) == "table" then
		local k,v
		for k,v in pairs(t) do
			print("  " .. k .. "=")
			print(v, indent .. "  ")
		end

		local mt = getmetatable(t)
		if mt ~= nil then
			print(indent .. "metatable:")
			Describe(mt, indent .. "  ")
		end
	else
		print(t)
	end
end

function TableSize(foo)
	local acc = 0
	for _ in pairs(foo) do
		acc = acc + 1
	end
	return acc
end

Game = {}
Draw = {}
Game.Input={}
Input = {}

print("luaclid.lua loaded")
