Selection = Selection or CreateNewClass("Selection")

function Selection:ctor()
end

function Selection:Clear(callback)
	for k,v in pairs(self) do
		if type(v) ~= "function" then
			self[k] = {}
		end
	end
	self.OnSelectionChange = callback or function() end
	self.OnSelectionChange(self.owner)
end

function Selection:Select(...)
	self:SelectInner(...)
	self.OnSelectionChange()
end

function Selection:SelectInner(kind, subkind, ...)
	self[kind] = self[kind] or {}
	if ... == nil then
		self[kind][subkind] = true
	else
		-- fucking with 'self'
		Selection.SelectInner(self[kind], subkind, ...)
	end
end

function Selection:Deselect(...)
	self:DeselectInner(...)
	self.OnSelectionChange()
end

function Selection:DeselectInner(kind, subkind, ...)
	self[kind] = self[kind] or {}
	if ... == nil then
		self[kind][subkind] = nil
	else
		--fucking with 'self'
		self.Select(self[kind], subkind, ...)
	end
end

function Selection:ToggleSelect(...)
	self:ToggleSelectInner(...)
	self.OnSelectionChange()
end

function Selection:ToggleSelectInner(kind, subkind, ...)
	self[kind] = self[kind] or {}
	if ... == nil then
		self[kind][subkind] = self[kind][subkind] == nil or nil
	else
		--fucking with 'self'
		Selection.ToggleSelect(self[kind], subkind, ...)
	end
end

function Selection:IsSelected(kind, subkind, ...)
	self[kind] = self[kind] or {}
	if ... == nil then
		return self[kind][subkind] or false
	else
		--fucking with 'self'
		return self.IsSelected(self[kind], subkind, ...)
	end
end

function Selection:GetSelected(kind)
	local ret = {}
	-- new version
	self:GetSelectedInner(ret, nil, kind)
	do return ret end

	-- old version
	for i in pairs(self[kind]) do
		table.insert(ret, i)
	end
	return ret
end

function Selection:GetSelectedInner(ret, base, kind)
	if self.kind then
		for i,v in pairs(self[kind]) do
			if type(v) == table then
				local newBase = DeepCopy(base or {})
				table.insert(newBase, i)
				self:GetSelectedInner(ret, newBase, self[kind])
			else
				if base == nil then
					table.insert(ret, i)
				else
					local item = DeepCopy(base)
					table.insert(item, v)
					table.insert(ret, item)
				end
			end
		end
	end
end

function Selection:GetSelected(kind)
	local ret = {}
	if self[kind] then
		self:GetSelectedInner(self[kind], ret)
	end
	return ret
end

function Selection:GetSelectedInner(base, ret, prefix)
	for k, v in pairs(base) do
		if type(v) == "table" then
			local newPrefix = DeepCopy(prefix or {})
			table.insert(newPrefix, k)
			self:GetSelectedInner(v, ret, newPrefix)
		else
			if prefix then
				local item = DeepCopy(prefix)
				table.insert(item, k)
				table.insert(ret, item)
			else
				table.insert(ret, k)
			end
		end
	end
end

function Selection:GetSelectedWalls()
	local ret = {}
	if self.walls then
		for i, s in pairs(self.walls) do
			for j in pairs(s) do
				table.insert(ret, {sec = i, wall = j})
			end
		end
	end
	return ret
end

