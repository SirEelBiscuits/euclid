Editor.ScaleSelectionState = Editor.ScaleSelectionState or
	CreateNewClass("ScaleSelectionState")
print("ScaleSelectionState reloaded")

function Editor.ScaleSelectionState:OnEnter()
	print("entering ScaleSelection state")

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

	self.epicentre = Maths.Vector:new(0,0,0)
	self.vertsOriginalPositions = {}
	local count = 0
	for i in pairs(self.vertsToMove) do
		count = count + 1
		self.vertsOriginalPositions[i] = Editor.curMapData.verts[i]
		self.epicentre = self.epicentre + Editor.curMapData.verts[i]
	end
	self.epicentre = self.epicentre / count

	self.curScale = 1
end

function Editor.ScaleSelectionState:DoScale(amount)
	for i in pairs(self.vertsToMove) do
		local vec = self.vertsOriginalPositions[i] - self.epicentre
		vec = vec * amount
		vec = vec + self.epicentre
		Editor.curMapData.verts[i] = vec
	end
end

function Editor.ScaleSelectionState:Update(dt)
	local mov = -Game.Controls.MouseYRel
	self.curScale = self.curScale * (1 + mov / 20)
	self:DoScale(self.curScale)

	if not Game.Controls.ScaleObject.isDown then
		Editor.curMapData:SetCentroids()
		self:PopState()
		return
	end

	if Editor.DragObjectState.WasMoveBad() then
		self.curScale = self.curScale / (1 + mov/20)
		self:DoScale(self.curScale)
	end
end

function Editor.ScaleSelectionState:Render()
	-- ?
	Editor.DefaultState:Render()
end

