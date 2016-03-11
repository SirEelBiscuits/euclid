History = History or CreateNewClass("History")

function History:ctor()
	self.level = 0
	self.snapshots = {}
end

function History:RegisterSnapshot()
	if self.level ~= 0 then
		local startIdx = #self.snapshots - self.level + 1
		print("Trimming history, " .. #self.snapshots .. " snapshots, trimming to level " .. self.level)
		for i = startIdx, #self.snapshots do
			self.snapshots[i] = nil
		end
	end
	self.level = 0

	self.owner.curMapData = DeepCopy(self.owner.curMapData)
	table.insert(self.snapshots, self.owner.curMapData)
end

function History:Undo()
	self.level = self.level + 1
	if self.level >= #self.snapshots then
		self.level = self.level - 1
	end
	self.owner.curMapData = self.snapshots[#self.snapshots - self.level]
end

function History:Redo()
	self.level = self.level - 1
	if self.level < 0 then self.level = 0 end
	self.owner.curMapData = self.snapshots[#self.snapshots - self.level]
end

function History:Clear()
	self.level = 0
	self.snapshots = {}
	self:RegisterSnapshot()
end
