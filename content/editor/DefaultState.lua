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
		Editor.DragObjectState:Enter()
	end

	if Game.Controls.DeleteObject.pressed then
		for i = #Editor.curMapData.verts, 1, -1 do
			if Editor.Selection:IsVertSelected(i) then
				Editor.curMapData:DeleteVert(i)
			end
		end

		Editor.Selection:Clear()
	end

	if Game.Controls.SplitWall.pressed then
		local walls = Editor.Selection:GetSelectedWalls()
		if #walls == 1 and #Editor.Selection:GetSelectedVerts() == 0 and #Editor.Selection:GetSelectedSectors() == 0 then
			Editor.Selection:Clear()
			Editor.Selection:SelectVert(Editor.curMapData:SplitWall(walls[1].sec, walls[1].wall))
		end
	end

end

function Editor.DefaultState.Render()
	if Editor.curMap ~= nil then
		Editor:DrawTopDownMap(
			{ 
				walls = {g = 128},
				vertSelection = {g = 255},
				sectorSelection = {g = 255},
				wallSelection = {b = 128},
			}
		)
	end

	if Textures.text then
		Draw.Text({x = 0, y = 0}, Textures.text, Editor.DebugText)
		Draw.Text({x = 64, y = 0}, Textures.text, tostring(Editor.Cursor.x) .. ", " .. tostring(Editor.Cursor.y))
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
		Draw.Text({x = 4, y = Draw.GetHeight() - 16}, Textures.text, info)
	elseif numSelected > 1 then
		Draw.Text({x = 4, y = Draw.GetHeight() - 16}, Textures.text, numSelected .. " verts selected")
	end
end

