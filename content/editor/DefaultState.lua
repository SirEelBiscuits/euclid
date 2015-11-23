Editor.DefaultState = Editor.DefaultState or {}
print("DefaultState reloaded")

function Editor.DefaultState:Enter(skipClear)
	skipClear = skipClear or false
	print("entering default state")
	if not skipClear then
		Editor.Selection:Clear(self.OnSelectionChanged)
	else
		self.OnSelectionChanged()
	end
	Editor.State = self
end

function Editor.DefaultState:Update(dt)
	Editor.Cursor = Editor:WorldFromScreen(Maths.Vector:new(Game.Controls.MouseX, Game.Controls.MouseY))

	if Game.Controls.reinit.pressed then
		Game.Initialise()
	end

	if Game.Controls.Quit.pressed then
		Game.quit = true
	end

	if Game.Controls.OpenMap.pressed then
		Editor.TypingState:Enter("Enter filename or hit escape", "Bad filename, try again",
			function(filename)
				Editor:OpenMap(filename)
				Editor.DefaultState:Enter()
			end
		)
	end

	if Game.Controls.Preview.pressed then
		Editor.PreviewState:Enter()
	end

	if Game.Controls.Save.pressed then
		print("saving")
		Editor.TypingState:Enter("Enter filename or hit escape", "Bad filename, try again",
			function (filename)
				Editor:SaveMap(filename)

				Editor.DefaultState:Enter()
			end
		)
	end

	if Game.Controls.Undo.pressed then
		Editor.Selection:Clear(self.OnSelectionChanged)
		Editor.History:Undo()
	end

	if Game.Controls.Redo.pressed then
		Editor.Selection:Clear(self.OnSelectionChanged)
		Editor.History:Redo()
	end

	if Game.Controls.AddSelect.pressed then
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

	if Game.Controls.ExclusiveSelect.pressed then
		Editor.Selection:Clear(self.OnSelectionChanged)
		
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

	if Game.Controls.DragObject.isDown then
		Editor.History:RegisterSnapshot()
		Editor.DragObjectState:Enter()
	end

	if Game.Controls.DeleteObject.pressed then
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

		Editor.Selection:Clear(self.OnSelectionChanged)
	end

	if Game.Controls.SplitWall.pressed then
		local walls = Editor.Selection:GetSelectedWalls()
		if #walls > 0 and #Editor.Selection:GetSelectedVerts() == 0 and #Editor.Selection:GetSelectedSectors() == 0 then
			Editor.History:RegisterSnapshot()
			Editor.Selection:Clear(self.OnSelectionChanged)
			table.sort(walls, function(a, b) return a.sec < b.sec or (a.sec == b.sec and a.wall < b.wall) end )
			for i = #walls, 1, -1 do
				local v = Editor.curMapData:SplitWall(walls[i].sec, walls[i].wall)
				Editor.Selection:SelectVert(v)
			end
		end
	end

	if Game.Controls.JoinSectors.pressed then
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
			Editor.Selection:Clear(self.OnSelectionChanged)
		elseif #verts == 0 and #walls == 0 and #sects == 2 then
			Editor.History:RegisterSnapshot()
			Editor.Selection:Clear(self.OnSelectionChanged)
			local idx = Editor.curMapData:JoinSectors(sects[1], sects[2])
			if idx ~= nil then
				Editor.Selection:SelectSector(idx)
			end
		end
	end

	if Game.Controls.EnterDrawSectorMode.pressed then
		Editor.History:RegisterSnapshot()
		Editor.DrawSectorState:Enter()
	end

	if Game.Controls.SetCeilHeight.pressed then
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

	if Game.Controls.SetFloorHeight.pressed then
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

	if Game.Controls.SetCeilTexture.pressed then
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

	if Game.Controls.SetFloorTexture.pressed then
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

	if Game.Controls.SetTopTexture.pressed then
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

	if Game.Controls.SetMainTexture.pressed then
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

	if Game.Controls.SetBottomTexture.pressed then
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

	if Game.Controls.DragCamera.isDown then
		Editor.CameraDragState:Enter()
	end
end

function Editor.DefaultState.OnSelectionChanged()
	Editor.DefaultState.InfoString, Editor.DefaultState.InfoStringHeight, Editor.DefaultState.InfoStringTextures = Editor:GetSelectionString()
end

function Editor.DefaultState:Render()
	Editor:DrawTopDownMap(Editor.Colors)

	if Textures.text then
		local CursorString = tostring(Editor.Cursor.x) .. ", " .. tostring(Editor.Cursor.y)
		Draw.Text({x = Draw.GetWidth() - 8 * #CursorString - 4, y = Draw.GetHeight() - 20}, Textures.text, CursorString)
		self:SelectionInfo()
	end
end

function Editor.DefaultState:SelectionInfo()
	local drawPos = Maths.Vector:new(4, Draw.GetHeight() - 16 * self.InfoStringHeight - 4)
	Draw.Text(drawPos, Textures.text, self.InfoString)
	for i, info in ipairs(self.InfoStringTextures) do
		local pos = drawPos + info.pos
		pos.w, pos.h = info.pos.w, info.pos.h
		Draw.RectTextured(pos, Draw.GetTexture(info.tex))
	end
end

