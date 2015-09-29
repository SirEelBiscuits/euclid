local function char(c)
	return ("\\%03d"):format(c:byte())
end

local function serializeInner (o, indent)
	local ret = ""
	indent = indent or ""
	if type(o) == "number" then
		return tostring(o)

	if type(o) == "boolean" then
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

	elseif type(o) == "function" then
		ret = ret .. indent .. 'load("' .. string.dump(o) .. '")'

	elseif type(o) == "nil" then
		ret = ret .. "nil"

	else
		error("cannot serialize a " .. type(o))
	end
	return ret
end

function serialize(o)
	return "return " .. serializeInner(o, "  ") .. "\n"
end
