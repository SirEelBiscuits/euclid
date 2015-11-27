Editor.TexturePickerState = Editor.TexturePickerState or {}

function Editor.TexturePickerState:Enter(prompt, callback)
	print("Entering texture picker state")

	if type(prompt) == "boolean" and prompt == false then
		Editor.State = self
		return 
	end

	self.prompt = prompt
	self.callback = callback
	self.curPage = 1
	self.cursor = 1

	self.textures = {[""] = Draw.GetTexture("")}

	for i, sec in ipairs(Editor.curMapData.sectors) do
		local texName
		if sec.floorTex ~= nil then
			texName = sec.floorTex.tex
			if texName ~= nil then
				self.textures[texName] = Draw.GetTexture(texName)
			end
		end
		if sec.ceilTex ~= nil then
			texName = sec.ceilTex.tex
			if texName ~= nil then
				self.textures[texName] = Draw.GetTexture(texName)
			end
		end

		for j, wall in ipairs(sec.walls) do
		if wall.bottomTex ~= nil then
			texName = wall.bottomTex.tex
			if texName ~= nil then
				self.textures[texName] = Draw.GetTexture(texName)
			end
		end
		if wall.mainTex ~= nil then
			texName = wall.mainTex.tex
			if texName ~= nil then
				self.textures[texName] = Draw.GetTexture(texName)
			end
		end
		if wall.topTex ~= nil then
			texName = wall.topTex.tex
			if texName ~= nil then
				self.textures[texName] = Draw.GetTexture(texName)
			end
		end

		end

	end

	local numTex = TableSize(self.textures)
	print (numTex .. " textures collected")

	Editor.State = self
end

function Editor.TexturePickerState:Update()
	if Game.Controls.CancelTexturePick.pressed then
		self.callback(false)
	end

	if Game.Controls.TexturePickerAddNew.pressed then
		Editor.TypingState:Enter("Filename of new texture", "Try again",
			function(string)
				self.textures[string] = Draw.GetTexture(string)

				self:Enter(false)
			end
		)
	end

	if Game.Controls.TexturePickerSelect.pressed then
		local ret = nil
		local acc = 0
		for name in pairs(self.textures) do
			acc = acc + 1
			if acc == self.cursor then
				ret = name
				break
			end
		end
		self.callback(ret or false)
	end

	if Game.Controls.TexturePickerLeft.pressed then
		self.cursor = self.cursor - 1
	end
	if Game.Controls.TexturePickerRight.pressed then
		self.cursor = self.cursor + 1
	end
	if Game.Controls.TexturePickerUp.pressed then
		self.cursor = self.cursor - Draw.GetWidth() // 64
	end
	if Game.Controls.TexturePickerDown.pressed then
		self.cursor = self.cursor + Draw.GetWidth() // 64
	end
	local entriesPerPage = ((Draw.GetWidth() // 64) * ((Draw.GetHeight() - 24) // 64))
	if Game.Controls.TexturePickerPageDown.pressed then
		self.cursor = self.cursor + entriesPerPage
	end
	
	local size = TableSize(self.textures)
	if size > 0 then
		while self.cursor < 1 do
			self.cursor = size + self.cursor
		end
		while self.cursor > size do
			self.cursor = self.cursor - size
		end
		self.curPage = self.cursor // entriesPerPage +1
	end
end

function Editor.TexturePickerState:Render()
	local dest = {x = 0, y = 24, w = 64, h = 64}
	local page = 1
	local cursorPos = 1
	local prompt = self.prompt
	for name, tex in pairs(self.textures) do
		if dest.x + dest.w > Draw.GetWidth() then
			dest.x = 0
			dest.y = dest.y + dest.h
		end
		if dest.y + dest.h > Draw.GetHeight() then
			page = page + 1
			if page > self.curPage then
				break
			end
		end

		if page == self.curPage then
			Draw.RectTextured(dest, tex)
		end

		if cursorPos == self.cursor then
			prompt = prompt .. " (" .. name .. ")"

			Draw.Line({x = dest.x, y = dest.y}, {x = dest.x + 63, y = dest.y}, Editor.Colors.selectedTexture)
			Draw.Line({x = dest.x + 63, y = dest.y}, {x = dest.x + 63, y = dest.y + 63}, Editor.Colors.selectedTexture)
			Draw.Line({x = dest.x + 63, y = dest.y + 63}, {x = dest.x, y = dest.y + 63}, Editor.Colors.selectedTexture)
			Draw.Line({x = dest.x, y = dest.y + 63}, {x = dest.x, y = dest.y}, Editor.Colors.selectedTexture)
		end

		cursorPos = cursorPos + 1
		dest.x = dest.x + dest.w
	end

	Draw.Text({x = 4, y = 4}, Textures.text, prompt)
end
