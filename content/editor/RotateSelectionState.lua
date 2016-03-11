Editor.RotateSelectionState = Editor.RotateSelectionState or
	CreateNewClass("RotateSelectionState")
print("RotateSelectionState reloaded")

function Editor.RotateSelectionState:OnEnter()
	print("entering RotateSelection state")

	self.vertsToMove = {}
	local editor = self.machine.owner

	for i,v in pairs(editor.Selection.verts) do
		if v then
			self.vertsToMove[i] = true
		end
	end
	for i, s in pairs(editor.Selection.sectors) do
		if s then
			for j, w in ipairs(editor.curMapData.sectors[i].walls) do
				self.vertsToMove[w.start] = true
			end
		end
	end

	for i, w in pairs(editor.Selection.walls) do
		if s then
			local sec = editor.curMapData.sectors[i]
			for j, w in pairs(s) do
				if w then
					self.vertsToMove[sec.walls[j].start] = true
					self.vertsToMove[sec.walls[j % #sec.walls + 1].start] = true
				end
			end
		end
	end

	self.epicentre = editor.Cursor
	self.vertsOriginalPositions = {}
	for i in pairs(self.vertsToMove) do
		self.vertsOriginalPositions[i] = editor.curMapData.verts[i]
	end

	self.curAngle = 0
end

function Editor.RotateSelectionState:DoRotate(amount)
	local editor = self.machine.owner
	local mat = Maths.RotationMatrix(amount)
	for i in pairs(self.vertsToMove) do
		local vec = self.vertsOriginalPositions[i] - self.epicentre
		vec = mat * vec
		vec = vec + self.epicentre
		editor.curMapData.verts[i] = vec
	end
end

function Editor.RotateSelectionState:Update(dt)
	local editor = self.machine.owner
	local mov = -Game.Controls.MouseYRel
	self.curAngle = self.curAngle + mov / 20
	self:DoRotate(self.curAngle)

	if not Game.Controls.RotateObject.isDown then
		editor.curMapData:SetCentroids()
		self:PopState()
		return
	end

	if editor.DragObjectState.WasMoveBad(self) then
		self.curAngle = self.curAngle - mov / 20
		self:DoRotate(self.curAngle)
	end
end

function Editor.RotateSelectionState:Render()
	local editor = self.machine.owner
	editor.DefaultState.Render(self)
end

