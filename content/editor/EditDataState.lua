Editor.EditDataState = Editor.EditDataState or
	CreateNewClass("EditDataState")

function Editor.EditDataState:OnEnter(tableToEdit)
	Game.LoadControls(dofile("editor/Controls.lua"))
	self.tableToEdit = tableToEdit
	self.cursor = 1
	self.scrolledTo = 1
	self.screenLines = math.floor((Draw.GetHeight() / Draw.GetRenderScale()- 8 ) / 20)
	self:RecalcTableSize()
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

	local keys = sortedkeys(self.tableToEdit)
	local key = keys[self.cursor]
	if Game.Controls.DataEdit.pressed then
		local value = self.tableToEdit[key]
		local t = type(value)
		if t == "table" then
			self:PushState(Editor.EditDataState, self.tableToEdit[key])
		elseif t == "number" then
			self:PushState(Editor.TypingState, "Enter number for " .. tostring(key),
				"???",
				function(val)
					self.tableToEdit[key] = tonumber(val)
					self:PopState()
				end
			)
		elseif t == "string" then
			self:PushState(Editor.TypingState, "Enter string for " .. tostring(key),
				"???",
				function(string)
					self.tableToEdit[key] = string
					self:PopState()
				end
			)
		elseif t == "boolean" then
			self.tableToEdit[key] = not value
		end
	end

	if Game.Controls.DataNewTable.pressed then
		self:PushState(Editor.TypingState, "Enter table name", "???",
			function(name)
				self.tableToEdit[name] = {}
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end
	if Game.Controls.DataNewNumber.pressed then
		self:PushState(Editor.TypingState, "Enter number name", "???",
			function(name)
				self.tableToEdit[name] = 0
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end
	if Game.Controls.DataNewString.pressed then
		self:PushState(Editor.TypingState, "Enter string name", "???",
			function(name)
				self.tableToEdit[name] = ""
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end
	if Game.Controls.DataNewBool.pressed then
		self:PushState(Editor.TypingState, "Enter bool name", "???",
			function(name)
				self.tableToEdit[name] = false
				self:RecalcTableSize()
				self:PopState()
			end
		)
	end

	if Game.Controls.DataDelete.pressed then
		self.tableToEdit[key] = nil
		self:RecalcTableSize()
	end

	return true
end

function Editor.EditDataState:Render()
	Draw.Rect({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, {})
	local pos = Maths.Vector:new(4,4)
	local curLine = self.scrolledTo
	for k, v in sortedpairs(self.tableToEdit, self.scrolledTo) do
		if curLine == self.cursor then
			Draw.Text(pos, Textures.text, ">")
		end
		pos.x = pos.x + 20
		Draw.Text(pos, Textures.text, self:Describe(k, v))

		pos.x = 4
		pos.y = pos.y + 20
		curLine = curLine + 1
		if pos.y + 16 > Draw.GetHeight() - 4 then
			break
		end
	end
end

function Editor.EditDataState:Describe(k, v)
	return k .. " [" .. type(v) .. "] -> " .. tostring(v)
end

function Editor.EditDataState:RecalcTableSize()
	self.tableSize = 0
	for _ in pairs(self.tableToEdit) do
		self.tableSize = self.tableSize + 1
	end
end
