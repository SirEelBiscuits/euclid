Editor.TypingGroupState = Editor.TypingGroupState or
	CreateNewClass("TypingGroupState")
print("TypingGroupState reloaded")

function Editor.TypingGroupState:OnEnter(prompt, options, failMessage, callback)
	print("Starting to type group")
	self.callback = callback or nullfunc
	self.prompt = prompt
	self.failMessage = failMessage
	self.typedString = ""
	self.options = options
	Input.SetTextEditingMode(true)

	self:UpdateOptions()
end

function Editor.TypingGroupState:Update(dt)
	for i, v in ipairs(Game.Input) do
		if not v.keyRepeat and v.eventType == InputEventType.KeyDown then
			self.failed = false

			if v.key == 8 then
				self.typedString = self.typedString:sub(1, self.typedString:len() - 1)
				self:UpdateOptions()
			elseif v.key == 27 then
				self:PopState("TypingGroupStateCancel")
				return
			elseif v.key < 32 then
				local res, err = pcall(self.callback, self.typedString)
				if res then
					Input.SetTextEditingMode(false)
				else
					self.err = err
					self.failed = true
				end
			end
		elseif v.eventType == InputEventType.TextInput then
			if utf8.codepoint(v.textInput:sub(1,1)) >= 32 then
				self.typedString = self.typedString .. v.textInput
				self:UpdateOptions()
			end
		end
	end

	return false
end

function Editor.TypingGroupState:Render()
	if Textures.text then
		Draw.Text({x = 4, y = 4}, Textures.text, not self.failed and self.prompt or self.failMessage )
		Draw.Text({x = 4, y = 24}, Textures.text, "> " .. self.typedString)

		if self.failed then
			Draw.Text({x = 4, y = 50}, Textures.text, self.err)
		end

		local pos = Maths.Vector:new(4, 36)
		if self.curOptions then
			for k, v in pairs(self.curOptions) do
				Draw.Text(pos, Textures.text, v)
				pos.y = pos.y + 16
				if pos.y > Draw.GetHeight() - 20 then
					break
				end
			end
		end
	end
end

function Editor.TypingGroupState:UpdateOptions()
	if not self.options then return end
	local newOptions = {}
	for k,v in pairs(self.options) do
	 if k:sub(1, #self.typedString) == self.typedString then
		 table.insert(newOptions, k)
	 end
	end
	self.curOptions = newOptions
end
