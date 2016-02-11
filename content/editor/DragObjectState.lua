Editor.DragObjectState = Editor.DragObjectState or CreateNewClass("DragObjectState")
print("DragObjectState reloaded")

function Editor.DragObjectState:OnEnter()
	print("entering dragobject state")

	self.vertsToMove = {}
	local editor = self.machine.owner

	for i, v in pairs(editor.Selection.verts) do
		if v == true then
			self.vertsToMove[i] = true
		end
	end

	for i, s in pairs(editor.Selection.sectors) do
		if s == true then
			for j, wall in ipairs(editor.curMapData.sectors[i].walls) do
				self.vertsToMove[wall.start] = true
			end
		end
	end

	for i, s in pairs(editor.Selection.walls) do
		if s ~= nil then
			local sec = editor.curMapData.sectors[i]
			for j, w in pairs(s) do
				if w == true then
					self.vertsToMove[sec.walls[j].start] = true
					self.vertsToMove[sec.walls[j % #sec.walls + 1].start] = true
				end
			end
		end
	end
end

function Editor.DragObjectState:DoMove(x, y)
	local editor = self.machine.owner
	local xmov = x / editor.view.scale / Draw.GetScale()
	local ymov = -y / editor.view.scale / Draw.GetScale()
	for i, v in ipairs(self.machine.owner.curMapData.verts) do
		if self.vertsToMove[i] then
			v.x = v.x + xmov
			v.y = v.y + ymov
		end
	end

	for i, s in pairs(editor.Selection.sectors) do
		local sec = editor.curMapData.sectors[i]
		sec.centroid.x = sec.centroid.x + xmov
		sec.centroid.y = sec.centroid.y + ymov
	end

	if editor.Selection.sprites then
		for i, sec in pairs(editor.Selection.sprites) do
			local sector = editor.curMapData.sectors[i]
			if not editor.Selection:IsSelected("sectors", i) then
				for j, spr in pairs(sec) do
					local sprite = sector.sprites[j]
					sprite.offset.x = sprite.offset.x + xmov
					sprite.offset.y = sprite.offset.y + ymov
				end
			end
		end
	end
end

function Editor.DragObjectState:WasMoveBad()
	local editor = self.machine.owner
	for i in ipairs(editor.curMapData.sectors) do
		if not editor.curMapData:IsSectorConvex(i) then
			return true
		end
	end

	if editor.Selection.sprites then
		for i, sec in pairs(editor.Selection.sprites) do
			local sector = editor.curMapData.sectors[i]
			for j, spr in pairs(sec) do
				local sprite = sector.sprites[j]
				if not editor.curMapData:IsPointInSector(
						sector.centroid + sprite.offset, i
					)
				then
					return true
				end
			end
		end
	end

	return false
end

function Editor.DragObjectState:Update(dt)
	if not Game.Controls.DragObject.isDown then
		self.machine.owner.curMapData:SetCentroids()
		self:PopState()
		return
	end

	self:DoMove(Game.Controls.MouseXRel, Game.Controls.MouseYRel)
	if self:WasMoveBad() then
		self:DoMove(-Game.Controls.MouseXRel, -Game.Controls.MouseYRel)
	end
end

function Editor.DragObjectState:Render()
	Editor.DefaultState.Render(self)
end
