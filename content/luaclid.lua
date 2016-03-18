-- Things in this file are needed before native-side code setup happens.
-- If you dick with this file, bad things can happen. I'm not kidding.

function MakeEnum(table, name)
	return NameObject(setmetatable({}, {
		__index = table,
		__newindex = function()
			if name then
				error("Attempted to modify" .. name .. " enum")
			else
				error("Attempted to modify enum")
			end
		end,
		__metatable = false
	}), name or "unknown enum")
end

local WeakMetaTable = { __mode = "v" }
local WeakKeyMetaTable = { __mode = "k" }

function NewWeakTable(o)
	return setmetatable(o or {}, WeakMetaTable)
end

function NewWeakKeyTable(o)
	return setmetatable(o or {}, WeakKeyMetaTable)
end

local Names = NewWeakKeyTable()

function NameObject(o, name)
	Names[o] = name
	return o
end

function GetName(o)
	return Names[o] or tostring(o)
end

ClassType = {}
NameObject(ClassType, "Class")

function CreateNewClass(baseClass, name)
	if type(baseClass) == "string" then
		baseClass, name = name, baseClass
	end
	local Class = {Super = baseClass}
	Class.Class = Class
	setmetatable(Class, ClassType)
	NameObject(Class, name)
	Class.__index = Class

	function Class:new(o)
		local NewInst = o or {}
		setmetatable(NewInst, Class)
		if(NewInst.ctor) then
			NewInst:ctor()
		end
		if o and o.name then
			NameObject(NewInst, o.name)
		elseif name then
			NameObject(NewInst, name .. ":" .. tostring(NewInst))
		end
		return NewInst
	end

	if baseClass then
		setmetatable(Class, {__index = baseClass})
	end
	return Class
end

NativeClassType = {}
NameObject(NativeClassType, "Native class?")

function CreateNativeClass(name)
	local Class = CreateNewClass()
	NameObject(Class, name or "Unknown Native Class")
	setmetatable(Class, NativeClassType)
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

Game       = NameObject({}, "Game")
Draw       = NameObject({}, "Draw")
Audio      = NameObject({}, "Audio")
Game.Input = NameObject({}, "Game.Input")
Input      = NameObject({}, "Input")

print("luaclid.lua loaded")
