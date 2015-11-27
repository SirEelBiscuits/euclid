Editor.TypingState = Editor.TypingState or {}
print("TypingState reloaded")

function Editor.TypingState:Enter(prompt, failMessage, callback)
	print("Starting to type string")
	Editor.State = self
	self.callback = callback or nullfunc
	self.prompt = prompt
	self.failMessage = failMessage
	self.returnState = returnState
	self.typedString = ""
	Input.SetTextEditingMode(true)
end

function Editor.TypingState:Update(dt)
	for i, v in ipairs(Game.Input) do
		if not v.keyRepeat and v.eventType == InputEventType.KeyDown then
			self.failed = false

			if v.key == 8 then
				self.typedString = self.typedString:sub(1, self.typedString:len() - 1)
			elseif v.key == 27 then
				Editor.DefaultState:Enter()
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
			end
		end
	end

	return false
end

function Editor.TypingState:Render()
	if Textures.text then
		Draw.Text({x = 4, y = 4}, Textures.text, not self.failed and self.prompt or self.failMessage )
		Draw.Text({x = 4, y = 24}, Textures.text, "> " .. self.typedString)

		if self.failed then
			Draw.Text({x = 4, y = 50}, Textures.text, self.err)
		end
	end
end

