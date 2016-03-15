Editor.EditDataState = Editor.EditDataState or
	CreateNewClass("EditDataState")

function Editor.EditDataState:OnEnter(tableToEdit, isMultiTable, path)
	Game.LoadControls(dofile("editor/Controls.lua"))
	self.isMultiTable = isMultiTable
	self.tableToEdit = tableToEdit
	self.cursor = 1
	self.scrolledTo = 1
	self.screenLines = math.floor((Draw.GetHeight() / Draw.GetRenderScale()- 8 ) / 20) - 1
	self:RecalcTableSize()
	self.path = path or GetName(tableToEdit)
end

function Editor.EditDataState:GetKeyList()
	if self.isMultiTable then
		local keys = {}
		for _, v in pairs(self.tableToEdit) do
			for k in pairs(v) do
				keys[k] = true
			end
		end
		local keylist = {}
		for k in pairs(keys) do
			table.insert(keylist, k)
		end
		table.sort(keylist)
		return keylist
	else
		return sortedkeys(self.tableToEdit)
	end
end

function Editor.EditDataState:GetValue(keyname)
	if self.isMultiTable then
		local ret = nil
		local someNil = false
		local multipleValues = false
		for _, tbl in pairs(self.tableToEdit) do
			local v = tbl[keyname]
			if v == nil then
				someNil = true
			else
				if ret == nil then
					ret = v
				elseif ret ~= v then
					multipleValues = true
				end
			end
		end
		return multipleValues, ret, someNil
	else
		return false, self.tableToEdit[keyname]
	end
end

function Editor.EditDataState:SetValue(keyname, newValue)
	if self.isMultiTable then
		for _, tbl in pairs(self.tableToEdit) do
			tbl[keyname] = newValue
		end
	else
		self.tableToEdit[keyname] = newValue
	end
end

function Editor.EditDataState:GetType(keyname)
	if self.isMultiTable then
		local ret = nil
		local someNil = false
		for _, tbl in pairs(self.tableToEdit) do
			local t = type(tbl[keyname])
			if t == "nil" then
				someNil = true
			else
				if not ret then
					ret = t
				end
				if ret ~= nil and ret ~= t then
					ret = "multiple"
				end
			end
		end
		return ret, someNil
	else
		return type(self.tableToEdit[keyname]), false
	end
end

-- plan - show a list of all data members of the provided table
--        with a cursor to select data to edit
--        also a way to create new entries, and to specify their type
--        (out of string, number, table, bool)
--
--        Needs to support scrolling!

function Editor.EditDataState:Update(dt)
	-- the closures for calling Typing State do not behave nicely if this
	-- is called with self as the state machine. This fixes it.
	if self.State then self.State:Update(dt) return true end

	if Game.Controls.DataMoveDown.pressed then
		self.cursor = self.cursor + 1
	end
	if Game.Controls.DataMoveUp.pressed then
		self.cursor = self.cursor - 1
	end
	if Game.Controls.DataBack.pressed then
		self:PopState()
		return true
	end


	if self.cursor <= 0 then
		self.cursor = self.tableSize
	end
	if self.cursor < self.scrolledTo then
		self.scrolledTo = self.scrolledTo - self.screenLines
	end
	if self.cursor > self.tableSize then
		self.cursor = 1
	end
	if self.cursor >= self.scrolledTo + self.screenLines then
		self.scrolledTo = self.cursor
	end

	local keys = self:GetKeyList()
	local key = keys[self.cursor]
	if Game.Controls.DataEdit.pressed then
		local t = self:GetType(key)
		if t == "table" then
			if self.isMultiTable then
				local tableList = {}
				for _, tbl in pairs(self.tableToEdit) do
					tbl[key] = tbl[key] or {}
					table.insert(tableList, tbl[key])
				end
				self:PushState(Editor.EditDataState, tableList, true, self.path .. "." .. tostring(key))
			else
				self:PushState(Editor.EditDataState, self.tableToEdit[key], false, self.path .. "." .. tostring(key))
			end
		elseif t == "number" then
			self:PushState(Editor.TypingState, "Enter number for " .. tostring(key),
				"???",
				function(val)
					self:SetValue(key, tonumber(val))
					self:PopState()
				end
			)
		elseif t == "string" then
			self:PushState(Editor.TypingState, "Enter string for " .. tostring(key),
				"???",
				function(string)
					self:SetValue(key, string)
					self:PopState()
				end
			)
		elseif t == "boolean" then
			local multi, value, somenil = self:GetValue(key)
			if multi then
				self:SetValue(key, true)
			else
				if somenil then
					self:SetValue(key, value)
				else
					self:SetValue(key, not value)
				end
			end
		elseif t == "multiple" then
		end
	end

	if Game.Controls.DataNewTable.pressed then
		self:PushState(Editor.TypingState, "Enter table name", "???",
			function(name)
				if self.isMultiTable then
					for k, v in pairs(self.tableToEdit) do
						v[name] = {}
					end
				else
					self.tableToEdit[name] = {}
				end
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end
	if Game.Controls.DataNewNumber.pressed then
		self:PushState(Editor.TypingState, "Enter number name", "???",
			function(name)
				if self.isMultiTable then
					for k, v in pairs(self.tableToEdit) do
						v[name] = 0
					end
				else
					self.tableToEdit[name] = 0
				end
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end
	if Game.Controls.DataNewString.pressed then
		self:PushState(Editor.TypingState, "Enter string name", "???",
			function(name)
				if self.isMultiTable then
					for k, v in pairs(self.tableToEdit) do
						v[name] = ""
					end
				else
					self.tableToEdit[name] = ""
				end
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end
	if Game.Controls.DataNewBool.pressed then
		self:PushState(Editor.TypingState, "Enter bool name", "???",
			function(name)
				if self.isMultiTable then
					for k, v in pairs(self.tableToEdit) do
						v[name] = false
					end
				else
					self.tableToEdit[name] = false
				end
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end

	if Game.Controls.DataInsertTable.pressed then
		if self.isMultiTable then
			for k, v in pairs(self.tableToEdit) do
				table.insert(v, {})
			end
		else
			table.insert(self.tableToEdit, {})
		end
		self:RecalcTableSize()
	end
	if Game.Controls.DataInsertNumber.pressed then
		if self.isMultiTable then
			for k, v in pairs(self.tableToEdit) do
				table.insert(v, 0)
			end
		else
			table.insert(self.tableToEdit, {})
		end
		self:RecalcTableSize()
	end
	if Game.Controls.DataInsertString.pressed then
		if self.isMultiTable then
			for k, v in pairs(self.tableToEdit) do
				table.insert(v, "")
			end
		else
			table.insert(self.tableToEdit, {})
		end
		self:RecalcTableSize()
	end
	if Game.Controls.DataInsertBool.pressed then
		if self.isMultiTable then
			for k, v in pairs(self.tableToEdit) do
				table.insert(v, false)
			end
		else
			table.insert(self.tableToEdit, {})
		end
		self:RecalcTableSize()
	end

	if Game.Controls.DataDelete.pressed then
		if self.isMultiTable then
			for _, tbl in pairs(self.tableToEdit) do
				tbl[key] = nil
			end
		else
			self.tableToEdit[key] = nil
		end
		self:RecalcTableSize()
	end

	return true
end

function Editor.EditDataState:Render()
	Draw.Rect({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, {})

	local pos = Maths.Vector:new(4,4)

	Draw.Text(pos, Game.Text, self.path)

	pos.y = pos.y + 20

	local curLine = self.scrolledTo
	for i, k in ipairs(self:GetKeyList()) do
		if i >= curLine then
			if curLine == self.cursor then
				Draw.Text(pos, Game.Text, ">")
			end
			pos.x = pos.x + 20
			Draw.Text(pos, Game.Text, self:Describe(k))

			pos.x = 4
			pos.y = pos.y + 20
			curLine = curLine + 1
			if pos.y + 16 > Draw.GetHeight() - 4 then
				break
			end
		end
	end
end

function Editor.EditDataState:Describe(k)
	local multi, value, someNil = self:GetValue(k)
	if multi then
		value = "multiple values"
	else
		value = GetName(value)
	end
	if someNil then
		value = value .. "*"
	end
	local Type, someNilType = self:GetType(k)
	if someNilType then
		Type = Type .. "*"
	end
	return tostring(k) .. " [" .. Type .. "] -> " .. value
end

function Editor.EditDataState:RecalcTableSize()
	self.tableSize = #self:GetKeyList()
end
