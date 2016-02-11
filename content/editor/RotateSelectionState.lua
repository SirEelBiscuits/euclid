Editor.RotateSelectionState = Editor.RotateSelectionState or
	CreateNewClass("RotateSelectionState")
print("RotateSelectionState reloaded")

function Editor.RotateSelectionState:OnEnter()
	print("entering RotateSelection state")

	self.vertsToMove = {}

	for i,v in pairs(Editor.Selection.verts) do
		if v then
			self.vertsToMove[i] = true
		end
	end
	for i, s in pairs(Editor.Selection.sectors) do
		if s then
			for j, w in ipairs(Editor.curMapData.sectors[i].walls) do
				self.vertsToMove[w.start] = true
			end
		end
	end

	for i, w in pairs(Editor.Selection.walls) do
		if s then
			local sec = Editor.curMapData.sectors[i]
			for j, w in pairs(s) do
				if w then
					self.vertsToMove[sec.walls[j].start] = true
					self.vertsToMove[sec.walls[j % #sec.walls + 1].start] = true
				end
			end
		end
	end

	self.epicentre = Editor.Cursor
	self.vertsOriginalPositions = {}
	for i in pairs(self.vertsToMove) do
		self.vertsOriginalPositions[i] = Editor.curMapData.verts[i]
	end

	self.curAngle = 0
end

function Editor.RotateSelectionState:DoRotate(amount)
	local mat = Maths.RotationMatrix(amount)
	for i in pairs(self.vertsToMove) do
		local vec = self.vertsOriginalPositions[i] - self.epicentre
		vec = mat * vec
		vec = vec + self.epicentre
		Editor.curMapData.verts[i] = vec
	end
end

function Editor.RotateSelectionState:Update(dt)
	local mov = -Game.Controls.MouseYRel
	self.curAngle = self.curAngle + mov / 20
	self:DoRotate(self.curAngle)

	if not Game.Controls.RotateObject.isDown then
		Editor.curMapData:SetCentroids()
		self:PopState()
		return
	end

	if Editor.DragObjectState.WasMoveBad() then
		self.curAngle = self.curAngle - mov / 20
		self:DoRotate(self.curAngle)
	end
end

function Editor.RotateSelectionState:Render()
	Editor.DefaultState:Render()
end

