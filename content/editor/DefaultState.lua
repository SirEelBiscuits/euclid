Editor.DefaultState = Editor.DefaultState or {}
print("DefaultState reloaded")

function Editor.DefaultState:Enter()
	print("entering default state")
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
		Editor.TypingState:Enter(
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
		Editor.TypingState:Enter(
			function (filename)
				Editor:SaveMap(filename)

				Editor.DefaultState:Enter()
			end
		)
	end

	if Game.Controls.Undo.pressed then
		Editor.Selection:Clear()
		Editor.History:Undo()
	end

	if Game.Controls.Redo.pressed then
		Editor.Selection:Clear()
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
			local secIdx, wallIdx, dist = Editor:GetClosestWallIdx(Editor.Cursor,
				1 / Editor.view.scale)
			if secIdx ~= nil then
				if Editor.Selection:IsWallSelected(secIdx, wallIdx) then
					Editor.Selection:DeselectWall(secIdx, wallIdx)
				else
					Editor.Selection:SelectWall(secIdx, wallIdx)
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
		Editor.Selection:Clear()
		
		local idx, dist = Editor:GetClosestVertIdx(Editor.Cursor,
			1 / Editor.view.scale)
		if idx ~= nil then
			Editor.Selection:SelectVert(idx)
		else
			local secIdx, wallIdx, dist = Editor:GetClosestWallIdx(Editor.Cursor,
				1 / Editor.view.scale)
			if secIdx ~= nil then
				Editor.Selection:SelectWall(secIdx, wallIdx)
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

		Editor.Selection:Clear()
	end

	if Game.Controls.SplitWall.pressed then
		local walls = Editor.Selection:GetSelectedWalls()
		if #walls == 1 and #Editor.Selection:GetSelectedVerts() == 0 and #Editor.Selection:GetSelectedSectors() == 0 then
			Editor.History:RegisterSnapshot()
			Editor.Selection:Clear()
			Editor.Selection:SelectVert(Editor.curMapData:SplitWall(walls[1].sec, walls[1].wall))
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
			Editor.Selection:Clear()
		elseif #verts == 0 and #walls == 0 and #sects == 2 then
			Editor.History:RegisterSnapshot()
			Editor.Selection:Clear()
			Editor.Selection:SelectSector(Editor.curMapData:JoinSectors(sects[1], sects[2]))
		end
	end

	if Game.Controls.EnterDrawSectorMode.pressed then
		Editor.History:RegisterSnapshot()
		Editor.DrawSectorState:Enter()
	end
end

function Editor.DefaultState.Render()
	Editor:DrawTopDownMap(Editor.Colors)

	if Textures.text then
		local CursorString = tostring(Editor.Cursor.x) .. ", " .. tostring(Editor.Cursor.y)
		Draw.Text({x = Draw.GetWidth() - 8 * #CursorString - 4, y = Draw.GetHeight() - 20}, Textures.text, CursorString)
		SelectionInfo()
	end
end

function SelectionInfo()
	local numSelected = 0
	local info = ""
	for i in pairs(Editor.Selection.verts) do
		numSelected = numSelected + 1
		if numSelected > 1 then
			break
		end

		local v = Editor.curMapData.verts[i]

		info = "Selected vert: id = " .. i .. ", position: (".. v.x .. ", " .. v.y .. ")"
	end

	if numSelected == 1 then
		Draw.Text({x = 4, y = Draw.GetHeight() - 20}, Textures.text, info)
	elseif numSelected > 1 then
		Draw.Text({x = 4, y = Draw.GetHeight() - 20}, Textures.text, numSelected .. " verts selected")
	end
end

