Editor.Commands = Editor.Commands or {}

function Editor.Commands.reinit()
	Game.Initialise()
end

function Editor.Commands.Quit()
	Game.quit = true
end

function Editor.Commands.OpenMap()
	Editor.TypingState:Enter("Enter filename or hit escape", "Bad filename, try again",
		function(filename)
			Editor:OpenMap(filename)
			Editor.DefaultState:Enter()
		end
	)
end

function Editor.Commands.Preview()
	Editor.PreviewState:Enter()
end

function Editor.Commands.Save()
	print("saving")
	Editor.TypingState:Enter("Enter filename or hit escape", "Bad filename, try again",
		function (filename)
			Editor:SaveMap(filename)

			Editor.DefaultState:Enter()
		end
	)
end

function Editor.Commands.Undo()
	Editor.Selection:Clear(Editor.State.OnSelectionChanged)
	Editor.History:Undo()
end

function Editor.Commands.Redo()
	Editor.Selection:Clear(Editor.State.OnSelectionChanged)
	Editor.History:Redo()
end

function Editor.Commands.AddSelect()
	local idx, dist = Editor:GetClosestVertIdx(Editor.Cursor,
		1 / Editor.view.scale)
	if idx ~= nil then
		if Editor.Selection:IsVertSelected(idx) then
			Editor.Selection:DeselectVert(idx)
		else
			Editor.Selection:SelectVert(idx)
		end
	else
		local wallinfo = Editor:GetClosestWallIdx(Editor.Cursor,
			1 / Editor.view.scale)
		if #wallinfo > 0 then
			for i, info in ipairs(wallinfo) do
				if Editor.Selection:IsWallSelected(info.sec, info.wall) then
					Editor.Selection:DeselectWall(info.sec, info.wall)
				else
					Editor.Selection:SelectWall(info.sec, info.wall)
				end
			end
		else
			local SecUpdateList = {}
			local anyDeselected = false
			for i, sec in ipairs(Editor.curMapData.sectors) do
				if Editor.curMapData:IsPointInSector(Editor.Cursor, i) then
					table.insert(SecUpdateList, i)
					anyDeselected = anyDeselected or not Editor.Selection:IsSectorSelected(i)
				end
			end
			if anyDeselected then
				for i, v in ipairs(SecUpdateList) do
					Editor.Selection:SelectSector(v)
				end
			else
				for i, v in ipairs(SecUpdateList) do
					Editor.Selection:DeselectSector(v)
				end
			end
		end
	end
end

function Editor.Commands.ExclusiveSelect()
	Editor.Selection:Clear(Editor.State.OnSelectionChanged)

	local idx, dist = Editor:GetClosestVertIdx(Editor.Cursor,
		1 / Editor.view.scale)
	if idx ~= nil then
		Editor.Selection:SelectVert(idx)
	else
		local wallinfo = Editor:GetClosestWallIdx(Editor.Cursor,
			1 / Editor.view.scale)
		if #wallinfo > 0 then
			for i, info in ipairs(wallinfo) do
				Editor.Selection:SelectWall(info.sec, info.wall)
			end
		else
			for i, sec in ipairs(Editor.curMapData.sectors) do
				if Editor.curMapData:IsPointInSector(Editor.Cursor, i) then
					Editor.Selection:SelectSector(i)
				end
			end
		end
	end
end

function Editor.Commands.DeleteObject()
	if #Editor.Selection.verts > 0 or #Editor.Selection.sectors > 0 then
		Editor.History:RegisterSnapshot()
	end
	for i = #Editor.curMapData.sectors, 1, -1 do
		if Editor.Selection:IsSectorSelected(i) then
			Editor.curMapData:DeleteSector(i)
		end
	end
	for i = #Editor.curMapData.verts, 1, -1 do
		if Editor.Selection:IsVertSelected(i) then
			Editor.curMapData:DeleteVert(i)
		end
	end
	Editor.curMapData:DeleteUnreferencedVerts()

	Editor.Selection:Clear(Editor.State.OnSelectionChanged)
end

function Editor.Commands.SplitWall()
	local walls = Editor.Selection:GetSelectedWalls()
	if #walls > 0 and #Editor.Selection:GetSelectedVerts() == 0 and #Editor.Selection:GetSelectedSectors() == 0 then
		Editor.History:RegisterSnapshot()
		Editor.Selection:Clear(Editor.State.OnSelectionChanged)
		table.sort(walls, function(a, b) return a.sec < b.sec or (a.sec == b.sec and a.wall < b.wall) end )
		for i = #walls, 1, -1 do
			local v = Editor.curMapData:SplitWall(walls[i].sec, walls[i].wall)
			Editor.Selection:SelectVert(v)
		end
	end
end

function Editor.Commands.JoinSectors()
	local verts = Editor.Selection:GetSelectedVerts()
	local walls = Editor.Selection:GetSelectedWalls()
	local sects = Editor.Selection:GetSelectedSectors()
	if #verts == 2 and #walls == 0 and #sects == 0 then
		local UpdateList = {}
		for i, sec in ipairs(Editor.curMapData.sectors) do
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
			Editor.History:RegisterSnapshot()
		end
		for i = #UpdateList, 1, -1 do
			Editor.curMapData:SplitSector(UpdateList[i].sec, UpdateList[i].vert1, UpdateList[i].vert2)
		end
		Editor.Selection:Clear(Editor.State.OnSelectionChanged)
	elseif #verts == 0 and #walls == 0 and #sects == 2 then
		Editor.History:RegisterSnapshot()
		Editor.Selection:Clear(Editor.State.OnSelectionChanged)
		local idx = Editor.curMapData:JoinSectors(sects[1], sects[2])
		if idx ~= nil then
			Editor.Selection:SelectSector(idx)
		end
	end
end

function Editor.Commands.EnterDrawSectorMode()
	Editor.History:RegisterSnapshot()
	Editor.DrawSectorState:Enter()
end

function Editor.Commands.SetCeilHeight()
	local sects = Editor.Selection:GetSelectedSectors()

	if #sects > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TypingState:Enter("Enter new ceiling height", "bad number entered",
			function(string)
				local secs = Editor.Selection:GetSelectedSectors()
				for i, s in ipairs(secs) do
					local sec = Editor.curMapData.sectors[s]
					sec.ceilHeight = tonumber(string)
				end

				Editor.DefaultState:Enter(true)
			end
		)
	end
end

function Editor.Commands.SetFloorHeight()
	local sects = Editor.Selection:GetSelectedSectors()

	if #sects > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TypingState:Enter("Enter new floor height", "bad number entered",
			function(string)
				local secs = Editor.Selection:GetSelectedSectors()
				for i, s in ipairs(secs) do
					local sec = Editor.curMapData.sectors[s]
					sec.floorHeight = tonumber(string)
				end

				Editor.DefaultState:Enter(true)
		end
		)
	end
end

function Editor.Commands.SetLightLevel()
	local sects = Editor.Selection:GetSelectedSectors()

	if #sects > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TypingState:Enter("Enter new light level", "bad light level entered",
		function(string)
			local ll = tonumber(string)
			if ll < 0 or ll > 1 then
				error("Light level must be in 0-1 range")
			end
			for i, s in ipairs(sects) do
				local sec = Editor.curMapData.sectors[s]
				sec.lightLevel = ll
			end

			Editor.DefaultState:Enter(true)
		end
		)
	end
end

function Editor.Commands.SetCeilTexture()
	local sects = Editor.Selection:GetSelectedSectors()
	if #sects > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TexturePickerState:Enter("Pick Ceiling Texture",
			function(string)
				if type(string) == "string" then
					for i, s in ipairs(sects) do
						local sec = Editor.curMapData.sectors[s]
						sec.ceilTex = sec.ceilTex or {}
						sec.ceilTex.tex = string
					end
				end
				Editor.DefaultState:Enter(true)
			end
		)
	end
end

function Editor.Commands.SetFloorTexture()
	local sects = Editor.Selection:GetSelectedSectors()
	if #sects > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TexturePickerState:Enter("Pick Floor Texture",
			function(string)
				if type(string) == "string" then
					for i, s in ipairs(sects) do
						local sec = Editor.curMapData.sectors[s]
						sec.floorTex = sec.floorTex or {}
						sec.floorTex.tex = string
					end
				end
				Editor.DefaultState:Enter(true)
			end
		)
	end
end

function Editor.Commands.SetTopTexture()
	local walls = Editor.Selection:GetSelectedWalls()
	if #walls > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TexturePickerState:Enter("Pick Top Texture",
			function(string)
				if type(string) == "string" then
					for i, wall in ipairs(walls) do
						local wall = Editor.curMapData.sectors[wall.sec].walls[wall.wall]
						wall.topTex = wall.topTex or {}
						wall.topTex.tex = string
					end
				end
				Editor.DefaultState:Enter(true)
			end
		)
	end
end

function Editor.Commands.SetMainTexture()
	local walls = Editor.Selection:GetSelectedWalls()
	if #walls > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TexturePickerState:Enter("Pick Main Texture",
			function(string)
				if type(string) == "string" then
					for i, wall in ipairs(walls) do
						local wall = Editor.curMapData.sectors[wall.sec].walls[wall.wall]
						wall.mainTex = wall.mainTex or {}
						wall.mainTex.tex = string
					end
				end
				Editor.DefaultState:Enter(true)
			end
		)
	end
end

function Editor.Commands.SetBottomTexture()
	local walls = Editor.Selection:GetSelectedWalls()
	if #walls > 0 then
		Editor.History:RegisterSnapshot()
		Editor.TexturePickerState:Enter("Pick Bottom Texture",
			function(string)
				if type(string) == "string" then
					for i, wall in ipairs(walls) do
						local wall = Editor.curMapData.sectors[wall.sec].walls[wall.wall]
						wall.bottomTex = wall.bottomTex or {}
						wall.bottomTex.tex = string
					end
				end
				Editor.DefaultState:Enter(true)
			end
		)
	end
end

function Editor.Commands.PreviewRaiseCeiling(self)
	local ceilHeight = self.sector:get_ceilHeight() + 0.1
	self.sector:set_ceilHeight(ceilHeight)
	Editor.curMapData.sectors[self.secID].ceilHeight = ceilHeight
end

function Editor.Commands.PreviewLowerCeiling(self)
	local ceilHeight = self.sector:get_ceilHeight() - 0.1
	self.sector:set_ceilHeight(ceilHeight)
	Editor.curMapData.sectors[self.secID].ceilHeight = ceilHeight
end

function Editor.Commands.PreviewRaiseFloor(self)
	local floorHeight = self.sector:get_floorHeight() + 0.1
	self.sector:set_floorHeight(floorHeight)
	Editor.curMapData.sectors[self.secID].floorHeight = floorHeight
end

function Editor.Commands.PreviewLowerFloor(self)
	local floorHeight = self.sector:get_floorHeight() - 0.1
	self.sector:set_floorHeight(floorHeight)
	Editor.curMapData.sectors[self.secID].floorHeight = floorHeight
end

