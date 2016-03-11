Editor.TypingState = Editor.TypingState or CreateNewClass("TypingState")
print("TypingState reloaded")

function Editor.TypingState:OnEnter(prompt, failMessage, callback)
	print("Starting to type string")
	self.callback = callback or nullfunc
	self.prompt = prompt
	self.failMessage = failMessage
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
				self:PopState("TypingStateCancel")
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
			end
		end
	end

	return true
end

function Editor.TypingState:Render()
	Draw.Rect({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, {})
	if Game.Text then
		Draw.Text({x = 4, y = 4}, Game.Text, not self.failed and self.prompt or self.failMessage )
		Draw.Text({x = 4, y = 24}, Game.Text, "> " .. self.typedString)

		if self.failed then
			Draw.Text({x = 4, y = 50}, Game.Text, self.err)
		end
	end
end

