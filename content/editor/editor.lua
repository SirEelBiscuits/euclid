Editor = Editor or CreateNewClass("Editor")

Game.LoadAndWatchFile("editor/Selection.lua")
Game.LoadAndWatchFile("editor/History.lua")
Game.LoadAndWatchFile("editor/Commands.lua")
Game.LoadAndWatchFile("editor/DefaultState.lua")
Game.LoadAndWatchFile("editor/TypingState.lua")
Game.LoadAndWatchFile("editor/TypingGroupState.lua")
Game.LoadAndWatchFile("editor/DragObjectState.lua")
Game.LoadAndWatchFile("editor/DrawSectorState.lua")
Game.LoadAndWatchFile("editor/TexturePickerState.lua")
Game.LoadAndWatchFile("editor/CameraDragState.lua")
Game.LoadAndWatchFile("editor/ScaleSelectionState.lua")
Game.LoadAndWatchFile("editor/RotateSelectionState.lua")
Game.LoadAndWatchFile("editor/EditDataState.lua")


Editor.Colors = {
	walls           = {r =   0, g = 128, b =   0},
	step            = {r =  64, g =   0, b =   0},
	obstacle        = {r = 255, g =   0, b =   0},
	window          = {r =   0, g = 255, b = 255},
	portal          = {r =   0, g = 128, b = 128},
	connector       = {r =   0, g =  32, b =  32},
	weirdWall       = {r = 127, g = 127, b = 127},
	weirdPortal     = {r = 127, g = 127, b = 127},

	wallSelection   = {r =   0, g =   0, b = 128},

	vert            = {r =   0, g = 128, b =   0},
	vertSelection   = {r =   0, g =   0, b = 255},

	sectorSelection = {r =   0, g = 255, b =   0},
	sectorNonConvex = {r = 255, g =   0, b =   0},

	vertDrawing     = {r =   0, g = 255, b = 255},
	selectedTexture = {r =   0, g = 255, b =   0},

	object          = {r = 200, g = 200, b =  30},
	objectSelected  = {r =  30, g = 200, b = 200},
}

Editor.Lookup = {
	Sprite = { label = "Spr", color = {r = 200, g = 200, b = 30} },
	Spawner = { label = "Spwn", color = {r = 75, g = 125, b = 60} },
}

function Editor:ctor()
	self.Selection = Selection:new({owner = self})
	self.view = { eye = Maths.Vector:new(0, 0, 0), scale = 10, angle = 0 }
	self.Cursor = Maths.Vector:new(0,0,0)
	self.selectionRange = 3
	self.Clipboard = {}
	self.History = History:new({owner = self})

	self.StateMachine = StateMachine:new({owner = self})
end

function Editor.GetEmptyMap()
	local ret =  {
		sectors = {},
		verts = {}
	}
	MapUtility:SetUpMap(ret)
	return ret
end

function Editor:Quit()
	Game.quit = true
end

function Editor:OnEnter(map)
	Textures = { text = Draw.GetTexture("resources/Mecha.png") }

	self.Controls = dofile("editor/Controls.lua")
	Game.LoadControls(self.Controls)

	MapUtility.__index = MapUtility

	self.StateMachine:EnterState(Editor.DefaultState)
	if type(map) == "string" then
		map = dofile(map)
	end
	self.curMapData = map or self.GetEmptyMap()
	self.History:Clear()

	Game.quit = false

	Draw.SetRenderScale(1)
	Game.ShowMouse(true)

	print("Editor initialised")
end

function Editor:OnPopped()
	Draw.SetRenderScale(1)
	Game.LoadControls(self.Controls)
	Game.ShowMouse(true)
end

function Editor:Update(dt)
	self.FPS = self.FPS or 1
	local n = math.log(self.FPS)
	self.FPS = (n * self.FPS + 1/dt) / (n+1)

	self.Cursor = self:WorldFromScreen(
		Maths.Vector:new(
			Game.Controls.MouseX / Draw.GetScale(),
			Game.Controls.MouseY / Draw.GetScale()
		)
	)

	self.StateMachine:Update(dt)

	if false then
		if #Game.Input > 0 and not Game.Input[1].keyRepeat then
			print(Game.Input[1].key)
		end
	end

	return not Game.quit
end

function Editor:Render()
	Draw.Rect({x = 0, y = 0, w = Draw.GetWidth(), h = Draw.GetHeight()}, {})

	self.StateMachine:Render()

	if Textures.text then
		local s = tostring(math.floor(self.FPS))
		Draw.Text({x = Draw.GetWidth() - s:len() * 8 - 4, y = 4}, Textures.text, s)
	end
end

function Editor:EnterState(state, ...)
	self.StateMachine:EnterState(state, ...)
end

function Editor:PushState(state, ...)
	print("pushing state: " .. GetName(state))
	self.StateMachine:PushState(state, ...)
end

function Editor:PopState(...)
	self.StateMachine:PopState(...)
end

function Editor:OpenMap(filename)
	print("trying to open " .. filename)
	self.curMapName = filename
	self.curMapData = dofile(filename)
	MapUtility:SetUpMap(self.curMapData)

	self.curMapData:FixAllSectorWindings()

	self.curMap     = Game.OpenMap(self.curMapData)
	self.History:Clear()
	print("done")
end

function Editor:SaveMap(filename)
	if filename ~= "" then
		self.curMapName = filename or self.curMapName
	end
	print("trying to save " .. filename)
	local str = serialise(self.curMapData)
	local file = io.open(filename, "w")
	io.output(file)
	io.write(str)
	io.close(file)
	print("done")
end

function Editor:ScreenFromWorld(vec)
	local relative = (vec - self.view.eye) * self.view.scale
	relative.y = -relative.y
	relative = Maths.RotationMatrix(self.view.angle or 0) * relative
	return Maths.Vector:new(
		relative.x + Draw.GetWidth() / 2,
		relative.y + Draw.GetHeight() / 2
	)
end

function Editor:WorldFromScreen(vec)
	local relative = Maths.Vector:new(vec.x - Draw.GetWidth() / 2, vec.y - Draw.GetHeight() / 2)
	relative = Maths.RotationMatrix(-self.view.angle or 0) * relative
	relative.y = -relative.y
	return (relative / self.view.scale) + self.view.eye
end

local function MakeVec(v)
	return Maths.Vector:new(v.x, v.y, v.z)
end

function Editor:DrawTopDownMap(colors)
	if self.curMapData == nil then
		return
	end

	for i, sec in ipairs(self.curMapData.sectors) do
		if sec.objects then
			for j, obj in ipairs(sec.objects) do
				local pos = self:ScreenFromWorld(
					(obj.data.offset or Maths.Vector:new(0,0,0)) + sec.centroid
				)
				local radius = obj.data.radius or 0.5
				pos.x = pos.x - radius * self.view.scale * 0.7
				pos.y = pos.y - radius * self.view.scale * 0.7
				pos.w = radius * 1.4 * self.view.scale
				pos.h = radius * 1.4 * self.view.scale

				local Lookup = self.Lookup[obj.Class] or {label = "?", color = colors.object}

				local color = Lookup.color
				if self.Selection:IsSelected("objects", i, j) then
					color = colors.objectSelected
				end
				Draw.Rect(pos, color)
				local label = Lookup.label
				if pos.w > 8 * #obj.Class then
					Draw.Text(pos, Game.Text, obj.Class)
				elseif pos.w > 8 * #label then
					Draw.Text(pos, Game.Text, label)
				else
					Draw.Text(pos, Game.Text, obj.Class:sub(1,1))
				end
			end
		end
		for j, wall in ipairs(sec.walls) do
			local nWall = sec.walls[j % #sec.walls + 1]
			local color

			local walls = self:GetWallsWithEnds(wall.start, nWall.start)
			if #walls > 2 then
				color = colors.weirdWall
			elseif #walls == 2 then
				local sec1 = self.curMapData.sectors[walls[1].sec]
				local sec2 = self.curMapData.sectors[walls[2].sec]

				local wall1 = sec1.walls[walls[1].wall]
				local wall2 = sec2.walls[walls[2].wall]

				if wall1.portal == nil and wall2.portal == nil then
					color = colors.wall

				else
					if wall1.portal == nil or wall2.portal == nil then
						color = colors.weirdPortal
					else
						-- we've narrowed it down to portal, window, step, obstacle
						local portalBottom = math.max(sec1.floorHeight, sec2.floorHeight)
						local portalTop    = math.min(sec1.ceilHeight, sec2.ceilHeight)

						if portalBottom > portalTop then
							color = colors.weirdPortal
						else
							if portalTop - portalBottom <= 0.5 then
								color = colors.window
							else
								local diff = math.abs(sec1.floorHeight - sec2.floorHeight)
								if diff <= 0.05 then
									if sec1.floorTex == nil or sec2.floorTex == nil then
										color = colors.portal
									elseif sec1.floorTex.tex == sec2.floorTex.tex then
										color = colors.connector
									else
										color = colors.portal
									end
								elseif diff <= 0.5 then
									color = colors.step
								else
									color = colors.obstacle
								end

							end
						end
					end
				end
			elseif #walls == 1 then
				color = colors.walls
			else
				assert(false, "what the fuck?")
			end

			Draw.Line(
				self:ScreenFromWorld(MakeVec(self.curMapData.verts[wall.start])),
				self:ScreenFromWorld(MakeVec(self.curMapData.verts[nWall.start])),
				color
			)
		end
	end

	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local nWall = sec.walls[j % #sec.walls + 1]
			if self.Selection:IsSelected("walls", i, j) then
				Draw.Line(
					self:ScreenFromWorld(MakeVec(self.curMapData.verts[wall.start])),
					self:ScreenFromWorld(MakeVec(self.curMapData.verts[nWall.start])),
					colors.wallSelection
				)
			end
		end
	end

	for i, sec in ipairs(self.curMapData.sectors) do
		local v = self:ScreenFromWorld(sec.centroid)
		if self.curMapData:IsSectorConvex(i) == false then
			Draw.Rect(
				{
					x = v.x - 2,
					y = v.y - 2,
					w = 5,
					h = 5,
				},
				colors.sectorNonConvex
			)
		end
		if self.Selection:IsSelected("sectors", i) then
			Draw.Rect(
				{
					x = v.x - 1,
					y = v.y - 1,
					w = 3,
					h = 3
				},
				colors.sectorSelection
			)
		end
	end

	for i, vert in ipairs(self.curMapData.verts) do
		local color
		if self.Selection:IsSelected("verts", i) then
			color = colors.vertSelection
		else
			color = colors.vert
		end
		local v = self:ScreenFromWorld(MakeVec(vert))
		Draw.Rect(
			{
				x = v.x - 1,
				y = v.y - 1,
				w = 3,
				h = 3
			},
			color
		)
	end
end

function Editor:GetClosestVertIdx(vec, maxDist)
	local minDist = maxDist or 999999999999999
	local vertIdx = -1
	for i, vert in ipairs(self.curMapData.verts) do
		local diff2D = MakeVec(vert) - vec
		diff2D.z = 0
		local lenSq = diff2D:LengthSquared()
		if lenSq < minDist then
			minDist = lenSq
			vertIdx = i
		end
	end
	if vertIdx ~= -1 then
		return vertIdx, minDist
	end
end

function Editor:GetClosestWallIdx(vec, maxDist)
	local ret = {}
	local minDist = maxDist or 9999999999999999
	local secIdx = -1
	local wallIdx = -1
	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local start = self.curMapData:GetVert(wall.start)
			local wallEnd = self.curMapData:GetVert(sec.walls[j % #sec.walls + 1].start)
			local dist = Maths.PointLineSegDistance(vec, start, wallEnd)
			if
				dist < minDist
			then
				minDist = dist
				secIdx = i
				wallIdx = j
				ret = self:GetWallsWithEnds(wall.start, sec.walls[j % #sec.walls + 1].start)
			end
		end
	end

	return ret
end

function Editor:GetWallsWithEnds(vert1, vert2)
	local ret = {}
	for i, sec in ipairs(self.curMapData.sectors) do
		for j, wall in ipairs(sec.walls) do
			local wallend = sec.walls[j % #sec.walls + 1].start
			if
				(vert1 == wall.start and vert2 == wallend)
				or
				(vert1 == wallend and vert2 == wall.start)
			then
				table.insert(ret, {sec = i, wall = j})
			end
		end
	end
	return ret
end

function Editor:GetObjects(vec)
	local ret = {}
	for i, sec in ipairs(self.curMapData.sectors) do
		if sec.objects then
			for j,object in ipairs(sec.objects) do
				local rel = vec - (sec.centroid + (object.data.offset or Maths.Vector:new(0,0,0)))
				rel.z = 0
				if (rel):LengthSquared() < (object.radius or 0.5) ^ 2 then
					table.insert(ret, {i, j})
				end
			end
		end
	end
	return ret
end

function Editor:GetControlsKey(Name)
	local ret = ""
	for i, c in ipairs(self.Controls) do
		if c.Name == Name then
			if c.ShiftPressed then
				ret = "shift+"
			end
			if c.CtrlPressed then
				ret = ret .. "ctrl+"
			end
			return ret .. c.Key
		end
	end
end

function Editor:GetSelectionString()
	local verts = self.Selection:GetSelected("verts")
	local walls = self.Selection:GetSelectedWalls()
	local sectors = self.Selection:GetSelected("sectors")


	if #sectors > 0 and #verts == 0 and #walls == 0 then
		local sect = self.curMapData.sectors[sectors[1]]

		local sec1 = self.curMapData.sectors[sectors[1]]

		local ceilH = sec1.ceilHeight or "multiple values"
		local floorH = sec1.floorHeight or "multiple values"
		local light = sec1.lightLevel or "multiple values"
		local ceilTex = sec1.ceilTex ~= nil and sec1.ceilTex.tex or nil
		local floorTex = sec1.floorTex ~= nil and sec1.floorTex.tex or nil
		local group = sec1.group
		local someGroupless = group == nil
		local outdoors = sec1.outdoors
		for i,s in ipairs(sectors) do
			local sec = self.curMapData.sectors[s]
			if sec.ceilHeight ~= ceilH then
				ceilH = "multiple values"
			end
			if sec.floorHeight ~= floorH then
				floorH = "multiple values"
			end
			if sec.lightLevel ~= light then
				light = "multiple values"
			end
			if sec.ceilTex == nil or sec.ceilTex.tex ~= ceilTex then
				ceilTex = nil
			end
			if sec.floorTex == nil or sec.floorTex.tex ~= floorTex then
				floorTex = nil
			end
			if not group then
				group = sec.group
			else
				if sec.group and group ~= sec.group then
					group = "multiple values"
				end
				if sec.group == nil then
					someGroupless = true
				end
			end
			if (sec.outdoors == true) ~= (outdoors == true) then
				outdoors = "multiple values"
			end
		end

		local workingString = ""
		if #sectors == 1 then
			workingString = "Sector id: " .. sectors[1] .. "\n"
		else
			workingString = #sectors .. " sectors selected\n"
		end

		workingString = workingString
			.. "ceil height: " .. ceilH .. " [" .. (self:GetControlsKey("SetCeilHeight") or "???") .. "] to edit\n"
			.. "floor height: " .. floorH .. " [" .. (self:GetControlsKey("SetFloorHeight") or "???") .. "] to edit\n"
			.. "light level: " .. light .. " [" .. (self:GetControlsKey("SetLightLevel") or "???") .. "] to edit\n"
		local count = 4
		local ret = {}

		workingString = workingString .. "ceil Texture: [" .. (self:GetControlsKey("SetCeilTexture") or "???") .. "] to edit\n"
		count = count + 1
		if ceilTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = ceilTex })
			count = count + 4
		end

		workingString = workingString .. "floor Texture: [" .. (self:GetControlsKey("SetFloorTexture") or "???") .. "] to edit\n"
		count = count + 1
		if floorTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, {pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = floorTex})
			count = count + 4
		end

		workingString = workingString .. "group: " .. (group or "<nil>")
		if someGroupless then
			workingString = workingString .. "*"
		end
		workingString = workingString .. " [" .. (self:GetControlsKey("SetGroup") or "???") .. "] to edit\n"
		count = count + 1

		workingString = workingString .. "outdoors: " .. tostring(outdoors) .. " [" .. (self:GetControlsKey("SetOutdoors") or "???")
			.. "/" .. (self:GetControlsKey("SetIndoors") or "???") .. "] to change"
		count = count + 1

		return workingString, count, ret


	elseif #verts ~= 0 and #sectors == 0 and #walls == 0 then
		if #verts == 1 then
			local v = self.curMapData.verts[verts[1]]
			return "Selected vert: id = " .. verts[1] .. ", position: (" .. v.x .. ", " .. v.y .. ")", 1, {}
		else
			return #verts .. " verts selected", 1, {}
		end
	elseif #walls > 0 and #verts == 0 and #sectors == 0 then
		local wall1 = self.curMapData.sectors[walls[1].sec].walls[walls[1].wall]
		local topTex = wall1.topTex ~= nil and wall1.topTex.tex or nil
		if topTex == "" then topTex = nil end
		local mainTex = wall1.mainTex ~= nil and wall1.mainTex.tex or nil
		if mainTex == "" then mainTex = nil end
		local bottomTex = wall1.bottomTex ~= nil and wall1.bottomTex.tex or nil
		if bottomTex == "" then bottomTex = nil end

		for i, wall in ipairs(walls) do
			local wallref = self.curMapData.sectors[wall.sec].walls[wall.wall]
			if wallref.bottomTex == nil or wallref.bottomTex.tex ~= bottomTex then
				bottomTex = nil
			end
			if wallref.mainTex == nil or wallref.mainTex.tex ~= mainTex then
				mainTex = nil
			end
			if wallref.topTex == nil or wallref.topTex.tex ~= topTex then
				topTex = nil
			end
		end

		local workingString = ""

		if #walls == 1 then
			workingString = workingString .. "wall " .. walls[1].wall .. ", sector ".. walls[1].sec .. "\n"
		else
			workingString = workingString .. #walls .. " walls selected: \n"
		end

		local count = 1
		local ret = {}

		workingString = workingString .. "top Texture: [" .. (self:GetControlsKey("SetTopTexture") or "???") .. "] to edit\n"
		count = count + 1
		if topTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = topTex })
			count = count + 4
		end
		workingString = workingString .. "main Texture: [" .. (self:GetControlsKey("SetMainTexture") or "???") .. "] to edit\n"
		count = count + 1
		if mainTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = mainTex })
			count = count + 4
		end
		workingString = workingString .. "bottom Texture: [" .. (self:GetControlsKey("SetBottomTexture") or "???") .. "] to edit\n"
		count = count + 1
		if bottomTex ~= nil then
			workingString = workingString .. "\n\n\n\n"
			table.insert(ret, { pos = {x = 0, y = count * 16, w = 64, h = 64}, tex = bottomTex })
			count = count + 4
		end

		return workingString, count, ret
	else
		local working = ""
		if #verts > 0 then
			working = working .. #verts .. " vert"
			if #verts > 1 then
				working = working .. "s"
			end
		end
		if #walls > 0 then
			if #working > 0 then
				if #sectors == 0 then
					working = working .. " and "
				else
					working = working .. ", "
				end
			end
			working = working .. #walls .. " wall"
			if #walls > 1 then
				working = working .. "s"
			end
		end
		if #sectors > 0 then
			if #working > 0 then
				working = working .. " and "
			end
			working = working .. #sectors .. " sector"
			if #sectors > 1 then
				working = working .. "s"
			end
		end
		return working, 1, {}
	end

	return "", 0, {}
end

