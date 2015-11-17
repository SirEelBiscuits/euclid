Editor.DefaultState = Editor.DefaultState or {}
print("DefaultState reloaded")

function Editor.DefaultState:Enter()
	print("entering default state")
	Editor.State = self
end

function Editor.DefaultState:Update(dt)
	Editor.Cursor = Maths.Vector:new(Game.Controls.MouseX, Game.Controls.MouseY)

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

				Editor.DefaultStatea:Enter()
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
		end
	end

	if Game.Controls.ExclusiveSelect.pressed then
		Editor.Selection:Clear()
		
		local idx, dist = Editor:GetClosestVertIdx(Editor.Cursor,
			1 / Editor.view.scale)
		if idx ~= nil then
			Editor.Selection:SelectVert(idx)
		end
	end

	if Game.Controls.DragObject.isDown then
		Editor.DragObjectState:Enter()
	end
end

function Editor.DefaultState.Render()
	if Editor.curMap ~= nil then
		Editor:DrawTopDownMap(
			{ 
				walls = {g = 128},
				vertSelection = {g = 255},
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

