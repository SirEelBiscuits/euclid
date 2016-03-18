function sortedkeys(o)
	local keys = {}
	for k in pairs(o) do
		table.insert(keys, k)
	end
	table.sort(keys)
	return keys
end

function sortedpairs(o, startIndex)
	local keys = sortedkeys(o)
	local i = (startIndex or 1) - 1
	local size = #keys
	return function()
		i = i + 1
		if i <= size then
			return keys[i], o[keys[i]]
		end
	end
end

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
		lookup[obj] = copy
		for k,v in pairs(obj) do
			copy[k] = DeepCopy(v, lookup)
		end
		setmetatable(copy, getmetatable(obj))
	else
		copy = obj
	end
	return copy
end

function Describe(root, limit, leader, lookup)
	leader = leader or ""
	lookup = lookup or {}
	limit  = limit or 1
	lookup[root] = true
	if type(root) == "table" then
		for k, v in pairs(root) do
			if type(v) == "table" then
				if lookup[v] then
					print(leader .. tostring(k) .. " -> " .. GetName(v) .. "*")
				else
					lookup[v] = true
					if limit > 1 then
						print(leader .. tostring(k) .. " -> " .. GetName(v))
						Describe(v, limit - 1, leader .. "  ", lookup)
					else
						print(leader .. tostring(k) .. " -> " .. GetName(v) .. "+")
					end
				end
			else
				print(leader .. tostring(k) .. " -> " .. tostring(v))
			end
		end
	else
		print(leader .. GetName(root) .. " = " .. tostring(root))
	end

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
    for k,v in sortedpairs(o) do
			if isArr then
				ret = ret .. indent
			elseif type(k) == "string" then
				ret = ret .. indent .. k .. " = "
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

function TableSize(foo)
	local acc = 0
	for _ in pairs(foo) do
		acc = acc + 1
	end
	return acc
end

function count_all(f)
	local seen = {}
	local count_table
	count_table = function(t)
		if seen[t] then return end
		f(t)
		seen[t] = true
		for k,v in pairs(t) do
			if type(v) == "table" then
				count_table(v)
			elseif type(v) == "userdata" then
				f(v)
			end
		end
	end
	count_table(_G)
end

function type_count()
	local counts = {}
	local enumerate = function (o)
		local mt = getmetatable(o)
		local t = "unknown"
		if mt then
			t = GetName(getmetatable(o))
		else
			t = "unknown " .. type(o)
		end
		counts[t] = (counts[t] or 0) + 1
	end
	count_all(enumerate)
	return counts
end

function TypeStats()
	Describe(type_count())
end

function Explore(o)
	Game.StateMachine:PushState(Editor.EditDataState, o)
end

function CreateQuickSubClass(baseClass, name, defaults, components)
	local Class = CreateNewClass(baseClass, name)
	if defaults then
		for k,v in pairs(defaults) do
			Class[k] = v
		end
	end
	if components then
		Class.ctor = function(self)
			baseClass.ctor(self)
			for k, v in pairs(components) do
				local c = self:GetComponent(v.component)
				if c then
					for j, d in pairs(v.defaults) do
						c[j] = d
					end
				else
					self:AddNewComponent(v.component, v.defaults)
				end
			end
		end
	end
	return Class
end

function ClassFromName(name)
	local ClassCur = _G
	for word in name:gmatch('[^.]*') do
		if word ~= "" then
			ClassCur = ClassCur[word]
			if ClassCur == nil then
				return nil
			end
		end
	end
	return ClassCur
end

function ObjectsFromData(Object, skipCopy)
	local t = type(Object)
	if t == "table" then
		if Object.Class and Object.data then
			local o = Object.data
			if not skipCopy then
				o = DeepCopy(Object.data)
			end
			for k,v in pairs(o) do
				o[k] = ObjectsFromData(v)
			end
			local Class = ClassFromName(Object.Class)
			if Class and Class.new then
				return ClassFromName(Object.Class):new(o)
			else
				return nil
			end
		end
	end
	return Object
end

local profileData = {}
local timeStamp = 0
function StartProfile()
	debug.sethook(function (event)
		local f = debug.getinfo(2, 'f').func
		local e = profileData[f]
		if e == nil then
			local x = debug.getinfo(2, 'nS')
			e = {name = x.name, line = x.linedefined, source = x.source,
									 time = 0, count = 0}
			profileData[f] = e
		end
		if event == 'call' then
			e.time = e.time - os.clock()
			e.count = e.count + 1
		else
			e.time = e.time + os.clock()
		end
	end, "cr")
	timeStamp = timeStamp + os.clock()
end

function PauseProfile()
	timeStamp = timeStamp - os.clock()
	debug.sethook()
end

function DumpProfile()
	debug.sethook()
	local S = {}
	for k,v in pairs(profileData) do
		table.insert(S, v)
	end
	table.sort(S, function(a, b) return a.time < b.time end)
	for k,v in ipairs(S) do
		print(v.name, v.line, v.source, v.count, v.time)
	end
	print("total time: ", os.clock() - timeStamp)
	profileData = {}
end
