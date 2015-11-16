Editor.DragObjectState = Editor.DragObjectState or {}
print("DragObjectState reloaded")

function Editor.DragObjectState:Enter()
	print("entering dragobject state")
	Editor.State = self
end

function Editor.DragObjectState:Update(dt)
	Editor.Cursor = Maths.Vector:new(Game.Controls.MouseX, Game.Controls.MouseY)

	if not Game.Controls.DragObject.isDown then
		Editor.DefaultState:Enter()
	end

	for i,v in ipairs(Editor.curMapData.verts) do
		if Editor.Selection:IsVertSelected(i) then
			v.x = v.x + Game.Controls.MouseXRel / Editor.view.scale
			v.y = v.y + Game.Controls.MouseYRel / -Editor.view.scale
		end
	end

end

function Editor.DragObjectState:Render()
	Editor.DefaultState.Render()
end
