StateMachine = StateMachine or CreateNewClass("State Machine")

local EnterState = function(self, ...)
	return self.machine:EnterState(...)
end

local PushState = function(self, ...)
	return self.machine:PushState(...)
end

local PopState = function(self, ...)
	return self.machine:PopState(...)
end

function StateMachine:EnterState(state, ...)
	assert(state ~= nil)
	self.StateStack = {}
	if self.State and self.State.OnExit then
		self.State:OnExit()
	end

	--[[
	-- It might be possible to use the __index and __newindex overloads to
	-- redirect things from the state machine to the current state. This would
	-- simplify the interface a lot!
	]]--

	self:SetState(setmetatable({
		machine = self,
		EnterState = EnterState,
		PushState = PushState,
		PopState = PopState
	}, {__index = state}))
	if self.State.OnEnter then
		self.State:OnEnter(...)
	end
end

function StateMachine:PushState(state, ...)
	assert(state ~= nil)
	table.insert(self.StateStack, self.State)
	if self.State.OnPushed then
		self.State:OnPushed()
	end
	self:SetState(setmetatable({
		machine = self,
		EnterState = EnterState,
		PushState = PushState,
		PopState = PopState
	}, {__index = state}))
	if self.State.OnEnter then
		self.State:OnEnter(...)
	end
end

function StateMachine:PopState(...)
	if self.State.OnExit then
		self.State:OnExit()
	end
	self.SetState(table.remove(self.StateStack))
	if self.State.OnPopped then
		self.State:OnPopped(...)
	end
end

function StateMachine:SetState(state)
	self.State = state
	setmetatable(self, {__index = self.State, __newindex = self.State})
end

