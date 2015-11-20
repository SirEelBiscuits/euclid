Editor.TypingState = Editor.TypingState or {}
print("TypingState reloaded")

function Editor.TypingState:Enter(prompt, failMessage, callback)
	print("Starting to type string")
	Editor.State = self
	self.callback = callback or nullfunc
	self.prompt = prompt
	self.failMessage = failMessage
	self.typedString = ""
end

function Editor.TypingState:Update(dt)
	for i,v in ipairs(Game.Input) do
		if not v.keyRepeat and v.eventType == InputEventType.KeyDown then
			self.failed = false

			if v.key == 8 then
				self.typedString = self.typedString:sub(1, self.typedString:len() - 1)
			elseif v.key == 27 then
				Editor.DefaultState:Enter()
			elseif v.key < 32 then
				local res, err = pcall(self.callback, self.typedString)
				if res then
				else
					self.err = err
					self.failed = true
				end
			elseif v.key < 256 then
				self.typedString = self.typedString .. string.char(v.key)
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

