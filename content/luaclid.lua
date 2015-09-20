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
