Game.LoadAndWatchFile("game/SFA.lua")

Editor.Commands = Editor.Commands or {}

function Editor.Commands.reinit()
	Game.Initialise()
end

function Editor.Commands.Quit()
	Game.quit = true
end

function Editor.Commands.OpenMap(editor)
	editor:PushState(Editor.TypingState, "Enter filename or hit escape", "Bad filename, try again",
		function(filename)
			editor:OpenMap(filename)
			editor:PopState()
		end
	)
end

function Editor.Commands.Preview(editor)
	Draw.SetRenderScale(4)
	editor.machine:PushState(StateList.SFA, editor.curMapData)
end

function Editor.Commands.Save(editor)
	print("saving")
	editor:PushState(Editor.TypingState, "Enter filename or hit escape", "Bad filename, try again",
		function (filename)
			editor:SaveMap(filename)

			editor:PopState()
		end
	)
end

function Editor.Commands.Undo(editor)
	editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)
	editor.History:Undo()
end

function Editor.Commands.Redo(editor)
	editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)
	editor.History:Redo()
end

local function GetClicked(editor)
	local idx, dist = editor:GetClosestVertIdx(editor.Cursor,
		editor.selectionRange / editor.view.scale)
	if idx ~= nil then
		return "verts", 1, idx
	else
		local wallinfo = editor:GetClosestWallIdx(editor.Cursor,
			editor.selectionRange / editor.view.scale)
		if #wallinfo > 0 then
			return "walls", #wallinfo, wallinfo
		else
			local sprites = editor:GetSprites(editor.Cursor)
			if #sprites > 0 then
				return "sprites", #sprites, sprites
			else
				local secs = {}
				for i, sec in ipairs(editor.curMapData.sectors) do
					if editor.curMapData:IsPointInSector(editor.Cursor, i) then
						table.insert(secs, i)
					end
				end
				return "sectors", #secs, secs
			end
		end
	end
end


function Editor.Commands.AddSelect(editor)
	local kind, num, val = GetClicked(editor)
	if kind == "walls" then
		for i, info in ipairs(val) do
			editor.Selection:ToggleSelect("walls", info.sec, info.wall)
		end
	elseif kind == "sprites" then
		for i, spr in ipairs(val) do
			editor.Selection:ToggleSelect("sprites", spr[1], spr[2])
		end
	elseif kind == "verts" then
		editor.Selection:ToggleSelect("verts", val)
	elseif kind == "sectors" then
		local anyDeselected = false
		for i, sec in ipairs(val) do
			if not editor.Selection:IsSelected("sectors", sec) then
				anyDeselected = true
				break
			end
		end

		if anyDeselected then
			for i, sec in ipairs(val) do
				editor.Selection:Select("sectors", sec)
			end
		else
			for i, sec in ipairs(val) do
				editor.Selection:Deselect("sectors", sec)
			end
		end
	end
end

function Editor.Commands.ExclusiveSelect(editor)
	editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)

	local kind, num, val = GetClicked(editor)
	if kind == "verts" then
		editor.Selection:Select("verts", val)
	elseif kind == "sprites" then
		for i, spr in ipairs(val) do
			editor.Selection:ToggleSelect("sprites", spr[1], spr[2])
		end
	elseif kind == "walls" then
		for i, info in ipairs(val) do
			editor.Selection:Select("walls", info.sec, info.wall)
		end
	elseif kind == "sectors" then
		for i, sec in ipairs(val) do
			editor.Selection:Select("sectors", sec)
		end
	end
end

function Editor.Commands.DeleteObject(editor)
	if #editor.Selection.verts > 0 or #editor.Selection.sectors > 0
		or (editor.Selection.sprites and #editor.Selection.sprites > 0)
	then
		editor.History:RegisterSnapshot()
	end
	for i = #editor.curMapData.sectors, 1, -1 do
		if editor.Selection:IsSelected("sectors", i) then
			editor.curMapData:DeleteSector(i)
		else
			local sec = editor.curMapData.sectors[i]
			if sec.sprites then
				for j = #sec.sprites, 1, -1 do
					if editor.Selection:IsSelected("sprites", i, j) then
						table.remove(sec.sprites, j)
					end
				end
			end
		end
	end
	for i = #editor.curMapData.verts, 1, -1 do
		if editor.Selection:IsSelected("verts", i) then
			editor.curMapData:DeleteVert(i)
		end
	end
	editor.curMapData:DeleteUnreferencedVerts()

	editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)
end

function Editor.Commands.SplitWall(editor)
	local walls = editor.Selection:GetSelectedWalls()
	if #walls > 0 and #editor.Selection:GetSelected("verts") == 0 and #editor.Selection:GetSelected("sectors") == 0 then
		editor.History:RegisterSnapshot()
		editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)
		table.sort(walls, function(a, b) return a.sec < b.sec or (a.sec == b.sec and a.wall < b.wall) end )
		for i = #walls, 1, -1 do
			local v = editor.curMapData:SplitWall(walls[i].sec, walls[i].wall)
			editor.Selection:Select("verts", v)
		end
	end
end

function Editor.Commands.JoinSectors(editor)
	local verts = editor.Selection:GetSelected("verts")
	local walls = editor.Selection:GetSelectedWalls()
	local sects = editor.Selection:GetSelected("sectors")
	if #verts == 2 and #walls == 0 and #sects == 0 then
		local UpdateList = {}
		for i, sec in ipairs(editor.curMapData.sectors) do
			local vert1found = -1
			local vert2found = -1
			for j, wall in ipairs(sec.walls) do
				if wall.start == verts[1] then
					vert1found = j
				end
				if wall.start == verts[2] then
					vert2found = j
				end
			end


			if vert1found ~= -1 and vert2found ~= -1 then
				table.insert(UpdateList, { sec = i, vert1 = vert1found, vert2 = vert2found })
			end
		end
		if #UpdateList > 0 then
			editor.History:RegisterSnapshot()
		end
		for i = #UpdateList, 1, -1 do
			editor.curMapData:SplitSector(UpdateList[i].sec, UpdateList[i].vert1, UpdateList[i].vert2)
		end
		editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)
	elseif #verts == 0 and #walls == 0 and #sects == 2 then
		editor.History:RegisterSnapshot()
		editor.Selection:Clear(editor.StateMachine.State.OnSelectionChanged)
		local idx = editor.curMapData:JoinSectors(sects[1], sects[2])
		if idx ~= nil then
			editor.Selection:Select("sectors", idx)
		end
	end
end

function Editor.Commands.EnterDrawSectorMode(editor)
	editor.History:RegisterSnapshot()
	editor:PushState(Editor.DrawSectorState)
end

function Editor.Commands.SetCeilHeight(editor)
	local sects = editor.Selection:GetSelected("sectors")

	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TypingState, "Enter new ceiling height", "bad number entered",
			function(string)
				local secs = editor.Selection:GetSelected("sectors")
				for i, s in ipairs(secs) do
					local sec = editor.curMapData.sectors[s]
					sec.ceilHeight = tonumber(string)
				end

				editor:PopState()
			end
		)
	end
end

function Editor.Commands.SetGroup(editor)
	local options = {}
	for k,v in ipairs(editor.curMapData.sectors) do
		if v.group then
			options[v.group] = true
		end
	end
	editor:PushState(Editor.TypingGroupState, "Enter group name", options, "?",
		function(groupName)
			local secs = editor.Selection:GetSelected("sectors")
			for i, s in ipairs(secs) do
				local sec = editor.curMapData.sectors[s]
				sec.group = groupName
			end

			editor:PopState()
		end
	)
end

function Editor.Commands.SelectGroup(editor)
	local groups = {}
	local secs = editor.Selection:GetSelected("sectors")
	for _, s in pairs(secs) do
		local sec = editor.curMapData.sectors[s]
		if sec.group then
			groups[sec.group] = true
		end
	end
	for k, v in ipairs(editor.curMapData.sectors) do
		if groups[v.group] then
			editor.Selection:Select("sectors", k)
		end
	end
end

function Editor.Commands.AdjustCeilHeight(editor)
	local sects = editor.Selection:GetSelected("sectors")

	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TypingState, "Enter ceiling height adjustment", "bad number entered",
			function(string)
				local secs = editor.Selection:GetSelected("sectors")
				for i, s in ipairs(secs) do
					local sec = editor.curMapData.sectors[s]
					sec.ceilHeight = sec.ceilHeight + tonumber(string)
				end

				editor:PopState()
			end
		)
	end
end

function Editor.Commands.SetFloorHeight(editor)
	local sects = editor.Selection:GetSelected("sectors")

	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(editor.TypingState, "Enter new floor height", "bad number entered",
			function(string)
				local secs = editor.Selection:GetSelected("sectors")
				for i, s in ipairs(secs) do
					local sec = editor.curMapData.sectors[s]
					sec.floorHeight = tonumber(string)
				end

				editor:PopState()
		end
		)
	end
end

function Editor.Commands.AdjustFloorHeight(editor)
	local sects = editor.Selection:GetSelected("sectors")

	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TypingState, "Enter floor height adjustment", "bad number entered",
			function(string)
				local secs = editor.Selection:GetSelected("sectors")
				for i, s in ipairs(secs) do
					local sec = editor.curMapData.sectors[s]
					sec.floorHeight = sec.floorHeight + tonumber(string)
				end

				editor:PopState()
		end
		)
	end
end

function Editor.Commands.SetLightLevel(editor)
	local sects = editor.Selection:GetSelected("sectors")

	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TypingState, "Enter new light level", "bad light level entered",
		function(string)
			local ll = tonumber(string)
			if ll < 0 or ll > 1 then
				error("Light level must be in 0-1 range")
			end
			for i, s in ipairs(sects) do
				local sec = editor.curMapData.sectors[s]
				sec.lightLevel = ll
			end

			editor:PopState()
		end
		)
	end
end

function Editor.Commands.SetCeilTexture(editor)
	local sects = editor.Selection:GetSelected("sectors")
	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TexturePickerState, "Pick Ceiling Texture",
			function(string)
				if type(string) == "string" then
					for i, s in ipairs(sects) do
						local sec = editor.curMapData.sectors[s]
						sec.ceilTex = sec.ceilTex or {}
						sec.ceilTex.tex = string
					end
				end
				editor:PopState()
			end
		)
	end
end

function Editor.Commands.SetFloorTexture(editor)
	local sects = editor.Selection:GetSelected("sectors")
	if #sects > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TexturePickerState, "Pick Floor Texture",
			function(string)
				if type(string) == "string" then
					for i, s in ipairs(sects) do
						local sec = editor.curMapData.sectors[s]
						sec.floorTex = sec.floorTex or {}
						sec.floorTex.tex = string
					end
				end
				editor:PopState()
			end
		)
	end
end

function Editor.Commands.SetTopTexture(editor)
	local walls = editor.Selection:GetSelectedWalls()
	if #walls > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TexturePickerState, "Pick Top Texture",
			function(string)
				if type(string) == "string" then
					for i, wall in ipairs(walls) do
						local wall = editor.curMapData.sectors[wall.sec].walls[wall.wall]
						wall.topTex = wall.topTex or {}
						wall.topTex.tex = string
					end
				end
				editor:PopState()
			end
		)
	end
end

function Editor.Commands.SetMainTexture(editor)
	local walls = editor.Selection:GetSelectedWalls()
	if #walls > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TexturePickerState, "Pick Main Texture",
			function(string)
				if type(string) == "string" then
					for i, wall in ipairs(walls) do
						local wall = editor.curMapData.sectors[wall.sec].walls[wall.wall]
						wall.mainTex = wall.mainTex or {}
						wall.mainTex.tex = string
					end
				end
				editor:PopState()
			end
		)
	end
end

function Editor.Commands.SetBottomTexture(editor)
	local walls = editor.Selection:GetSelectedWalls()
	if #walls > 0 then
		editor.History:RegisterSnapshot()
		editor:PushState(Editor.TexturePickerState, "Pick Bottom Texture",
			function(string)
				if type(string) == "string" then
					for i, wall in ipairs(walls) do
						local wall = editor.curMapData.sectors[wall.sec].walls[wall.wall]
						wall.bottomTex = wall.bottomTex or {}
						wall.bottomTex.tex = string
					end
				end
				editor:PopState()
			end
		)
	end
end

-- this is screen pixels per world meter
Editor.Commands.ZoomLevels = {
	0.1,
	1,
	10,
	50,
	200
}
Editor.Commands.ZoomLevelID = Editor.Commands.ZoomLevelID or 3

local function ReZoom(editor)
	Editor.Commands.ZoomLevelID = math.min(#Editor.Commands.ZoomLevels, math.max(1, Editor.Commands.ZoomLevelID))
	editor.view.scale = Editor.Commands.ZoomLevels[Editor.Commands.ZoomLevelID]
end

function Editor.Commands.ZoomIn(editor)
	Editor.Commands.ZoomLevelID = Editor.Commands.ZoomLevelID + 1
	ReZoom(editor)
end

function Editor.Commands.ZoomOut(editor)
	Editor.Commands.ZoomLevelID = Editor.Commands.ZoomLevelID - 1
	ReZoom(editor)
end

function Editor.Commands.SetStartSector(editor)
	local secs = editor.Selection:GetSelected("sectors")
	if #secs == 1 then
		editor.History:RegisterSnapshot()
		editor.curMapData.defaultSector = secs[1]
		print("Start sector set")
	end
end

function Editor.Commands.Copy(editor)
	local copy = {
		verts = {},
		sectors = {},
	}
	copy.basePos = editor.view.eye

	-- METHOD
	-- we need to copy all verts, walls, sectors listed, and any objects they
	-- refer to.
	--
	-- This copy must be simple to paste new copies from so:
	-- verts should contain their x/y
	-- walls shouldn't be copied (they aren't true objects)
	-- sectors should cause any verts they use to be copied, but should refer to
	--  the vert in the copied verts list, not the global list!

	for i, v in ipairs(editor.Selection:GetSelected("verts")) do
		copy.verts[v] = DeepCopy(editor.curMapData.verts[v])
	end

	for i, s in pairs(editor.Selection:GetSelected("sectors")) do
		local sec = editor.curMapData.sectors[s]
		for j, w in ipairs(sec.walls) do
			if not copy.verts[w.start] then
				copy.verts[w.start] = DeepCopy(editor.curMapData.verts[w.start])
			end
		end
		copy.sectors[s] = DeepCopy(editor.curMapData.sectors[s])

		-- deal with portals here
	end

	editor.Clipboard.general = copy
end

function Editor.Commands.Paste(editor)
	local verthookup = {}
	local sechookup  = {}
	for i, v in pairs(editor.Clipboard.general.verts) do
		table.insert(
			editor.curMapData.verts,
			DeepCopy(v) - editor.Clipboard.general.basePos + editor.view.eye
		)
		verthookup[i] = #editor.curMapData.verts
	end

	for i, s in pairs(editor.Clipboard.general.sectors) do
		table.insert(editor.curMapData.sectors, DeepCopy(s))
		sechookup[i] = #editor.curMapData.sectors
		for j, w in ipairs(editor.curMapData.sectors[sechookup[i]].walls) do
			w.start = verthookup[w.start]
		end
		editor.curMapData:SetSectorCentroid(editor.curMapData.sectors[sechookup[i]])
	end

	for source, dest in pairs(sechookup) do
		for i, wall in ipairs(editor.curMapData.sectors[dest].walls) do
			if wall.portal then
				if sechookup[wall.portal] then
					wall.portal = sechookup[wall.portal]
				else
					wall.portal = nil
				end
			end
		end
	end

	editor.Selection:Clear()
	for _, s in pairs(sechookup) do
		editor.Selection:Select("sectors", s)
	end
	for _, v in pairs(verthookup) do
		editor.Selection:Select("verts", v)
	end
end

function Editor.Commands.CopyProperties(editor)
	local copy = {}
	local secs = editor.Selection:GetSelected("sectors")
	if #secs == 1 then
		copy.sectors = DeepCopy(editor.curMapData.sectors[secs[1]])
		copy.sectors.walls = nil
		copy.sectors.centroid = nil
	end
	local walls = editor.Selection:GetSelectedWalls()
	if #walls == 1 then
		copy.walls = DeepCopy(
			editor.curMapData.sectors[walls[1].sec].walls[walls[1].wall]
		)
		copy.walls.start = nil
		copy.walls.portal = nil
	end
	editor.Clipboard.properties = copy
end

function Editor.Commands.PasteProperties(editor)
	local copy = editor.Clipboard.properties
	if copy.sectors then
		local secs = editor.Selection:GetSelected("sectors")
		for _, s in ipairs(secs) do
			local sec = editor.curMapData.sectors[s]
			for k, v in pairs(copy.sectors) do
				sec[k] = v
			end
		end
	end
	if copy.walls then
		local walls = editor.Selection:GetSelectedWalls()
		for _, w in ipairs(walls) do
			local wall = editor.curMapData.sectors[w.sec].walls[w.wall]
			for k, v in pairs(copy.walls) do
				wall[k] = v
			end
		end
	end
end

function Editor.Commands.SetOutdoors(editor)
	local secs = editor.Selection:GetSelected("sectors")
	for _, v in ipairs(secs) do
		editor.curMapData.sectors[v].outdoors = true
	end
	editor.Selection.OnSelectionChange()
end

function Editor.Commands.SetIndoors(editor)
	local secs = editor.Selection:GetSelected("sectors")
	for _, v in ipairs(secs) do
		editor.curMapData.sectors[v].outdoors = false
	end
	editor.Selection.OnSelectionChange()
end

function Editor.Commands.SetOutdoorLevel(editor)
	editor:PushState(Editor.TypingState, "Enter value for 'fully lit by outdoor light'", "what?",
		function(value)
			local secs = editor.Selection:GetSelected("sectors")
			for _, s in ipairs(secs) do
				editor.curMapData.sectors[s].outdoors = tonumber(value)
			end

			editor:PopState()
		end
	)
end

function Editor.Commands.CreateSprite(editor)
	local secs = editor.Selection:GetSelected("sectors")
	if #secs ~= 1 then return end
	local sec = editor.curMapData.sectors[secs[1]]
	editor:PushState(Editor.TexturePickerState, "Pick sprite",
		function(sprite)
			sec.sprites = sec.sprites or {}
			table.insert(sec.sprites, {
				sprite = sprite,
				offset = Maths.Vector:new(0, 0, 0),
				radius = 1
			})
			editor:PopState()
		end
	)
end

function Editor.Commands.EditThing(editor)
	print("edit thing")
	local sprs = editor.Selection:GetSelected("sprites")
	if #sprs == 1 then
		local spr = editor.curMapData.sectors[sprs[1][1]].sprites[sprs[1][2]]
		editor:PushState(Editor.EditDataState, spr)
	else
		local secs = editor.Selection:GetSelected("sectors")
		if #secs == 1 then
			editor:PushState(Editor.EditDataState, editor.curMapData.sectors[secs[1]])
		end
	end
end

print("Commands reloaded")
