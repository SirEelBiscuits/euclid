Editor.DragObjectState = Editor.DragObjectState or {}
print("DragObjectState reloaded")

function Editor.DragObjectState:Enter()
	print("entering dragobject state")
	Editor.State = self
end

function Editor.DragObjectState:DoMove(x, y)
	local xmov = x / Editor.view.scale
	local ymov = -y / Editor.view.scale
	for i, v in ipairs(Editor.curMapData.verts) do
		if Editor.Selection:IsVertSelected(i) then
			v.x = v.x + xmov
			v.y = v.y + ymov
		end
	end
end

function Editor.DragObjectState:WasMoveBad()
	for i in ipairs(Editor.curMapData.sectors) do
		if not Editor.curMapData:IsSectorConvex(i) then
			return true
		end
	end
	return false
end

function Editor.DragObjectState:Update(dt)
	Editor.Cursor = Maths.Vector:new(Game.Controls.MouseX, Game.Controls.MouseY)

	if not Game.Controls.DragObject.isDown then
		print(Editor.curMapData:IsSectorConvex(1))
		Editor.DefaultState:Enter()
	end

	self:DoMove(Game.Controls.MouseXRel, Game.Controls.MouseYRel)
	if self:WasMoveBad() then
		self:DoMove(-Game.Controls.MouseXRel, -Game.Controls.MouseYRel)
	end

end

function Editor.DragObjectState:Render()
	Editor.DefaultState.Render()
end
