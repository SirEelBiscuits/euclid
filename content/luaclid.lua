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
	},
	"EventType"
)

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
    ret = ret .. ("{\n")
    for k,v in pairs(o) do
      ret = ret .. indent .. "[" .. serializeInner(k) .. "]" .. " = "
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

Game = {}

print("luaclid.lua loaded")
