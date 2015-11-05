Editor = Editor or { State = {}, TypingState = {}, DefaultState = {}}

function nullfunc()
end

function Game.Initialise()
	Textures = { text = Draw.GetTexture("Mecha.png") }

	Editor.State = Editor.DefaultState

	Game.LoadControls(dofile("editorcontrols.lua"))
	Game.quit = false

	--openMap("testmap.lua")

	print("initialised")
end

function Game.SaveState()
end
function Game.LoadState()
end

function Game.Quit()
	Game.quit = true;
end

function Game.Update(dt)
	Game.FPS = Game.FPS or 1
	local n = math.log(Game.FPS)
	Game.FPS = (n * Game.FPS + 1/dt) / (n+1)

	Editor.State:Update(dt)

	return not Game.quit
end

function Game.Render()
	Draw.Rect({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, {})

	Editor.State:Render()

	if Textures.text then
		local s = tostring(math.floor(Game.FPS))
		Draw.Text({x = Draw.GetWidth() - s:len() * 8 - 4, y = 4}, Textures.text, s)
	end
end

Editor.DefaultState = {}

function Editor.DefaultState:Enter()
	print("entering default state")
	Editor.State = self
end

function Editor.DefaultState:Update(dt)
	if Game.Controls.reinit.pressed then
		Game.Initialise()
	end

	if Game.Controls.Quit.pressed then
		Game.quit = true
	end

	if Game.Controls.OpenMap.pressed then
		Editor.TypingState:Enter(openMap)
	end
end

function Editor.DefaultState.Render()
	if Textures.text then
		Draw.Text({x = 0, y = 0}, Textures.text, Editor.DebugText)
	end
end

Editor.TypingState = {}

function Editor.TypingState:Enter(callback)
	print("Starting to type string")
	Editor.State = self
	self.callback = callback or nullfunc
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
					Editor.DefaultState:Enter()
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
		Draw.Text({x = 4, y = 4}, Textures.text, not self.failed and "Enter filename or hit escape" or "Bad filename, try again" )
		Draw.Text({x = 4, y = 24}, Textures.text, "> " .. self.typedString)

		if self.failed then
			Draw.Text({x = 4, y = 50}, Textures.text, self.err)
		end
	end
end

function openMap(filename)
	print("trying to open " .. filename)
	Editor.curMapName = filename
	Editor.curMapData = dofile(filename)
	Editor.curMap     = Game.OpenMap(Editor.curMapData)
	Editor.DebugText  = Editor.curMap:GetNumSectors()
	print("done")
end

