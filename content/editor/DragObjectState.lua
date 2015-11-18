Editor.DragObjectState = Editor.DragObjectState or {}
print("DragObjectState reloaded")

function Editor.DragObjectState:Enter()
	print("entering dragobject state")
	Editor.State = self

	self.vertsToMove = {}

	for i, v in pairs(Editor.Selection.verts) do
		if v == true then
			self.vertsToMove[i] = true
		end
	end

	for i, s in pairs(Editor.Selection.sectors) do
		if s == true then
			for j, wall in ipairs(Editor.curMapData.sectors[i].walls) do
				self.vertsToMove[wall.start] = true
			end
		end
	end
end

function Editor.DragObjectState:DoMove(x, y)
	local xmov = x / Editor.view.scale
	local ymov = -y / Editor.view.scale
	for i, v in ipairs(Editor.curMapData.verts) do
		if self.vertsToMove[i] then
			v.x = v.x + xmov
			v.y = v.y + ymov
		end
	end

	for i, sec in ipairs(Editor.curMapData.sectors) do
		sec.centroid.x = sec.centroid.x + xmov
		sec.centroid.y = sec.centroid.y + ymov
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
		Editor.curMapData:SetCentroids()
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
