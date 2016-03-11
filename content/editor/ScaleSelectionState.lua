Editor.ScaleSelectionState = Editor.ScaleSelectionState or
	CreateNewClass("ScaleSelectionState")
print("ScaleSelectionState reloaded")

function Editor.ScaleSelectionState:OnEnter()
	print("entering ScaleSelection state")

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

	self.epicentre = Maths.Vector:new(0,0,0)
	self.vertsOriginalPositions = {}
	local count = 0
	for i in pairs(self.vertsToMove) do
		count = count + 1
		self.vertsOriginalPositions[i] = editor.curMapData.verts[i]
		self.epicentre = self.epicentre + editor.curMapData.verts[i]
	end
	self.epicentre = self.epicentre / count

	self.curScale = 1
end

function Editor.ScaleSelectionState:DoScale(amount)
	local editor = self.machine.owner
	for i in pairs(self.vertsToMove) do
		local vec = self.vertsOriginalPositions[i] - self.epicentre
		vec = vec * amount
		vec = vec + self.epicentre
		editor.curMapData.verts[i] = vec
	end
end

function Editor.ScaleSelectionState:Update(dt)
	local editor = self.machine.owner
	local mov = -Game.Controls.MouseYRel
	self.curScale = self.curScale * (1 + mov / 20)
	self:DoScale(self.curScale)

	if not Game.Controls.ScaleObject.isDown then
		editor.curMapData:SetCentroids()
		self:PopState()
		return
	end

	if Editor.DragObjectState.WasMoveBad(self) then
		self.curScale = self.curScale / (1 + mov/20)
		self:DoScale(self.curScale)
	end
end

function Editor.ScaleSelectionState:Render()
	local editor = self.machine.owner
	editor.DefaultState.Render(self)
end

