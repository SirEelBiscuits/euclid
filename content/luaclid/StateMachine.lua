StateMachine = StateMachine or CreateNewClass("State Machine")

function StateMachine:ctor()

	-- This is done because the class dicks around with its metatable
	-- force setting these functions like this will prevent that breaking
	-- However! This will screw up file reloading if this file changes

	self.EnterState = StateMachine.EnterState
	self.PushState  = StateMachine.PushState
	self.PopState   = StateMachine.PopState
	self._SetState  = StateMachine._SetState
	self.State = {}
end

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

	self:_SetState(state:new({
		machine = self,
		EnterState = state.EnterState or EnterState,
		PushState  = state.PushState or PushState,
		PopState   = state.PopState or PopState
	}))
	if self.State.OnEnter then
		self.State:OnEnter(...)
	else
	end
end

function StateMachine:PushState(state, ...)
	assert(state ~= nil)
	table.insert(self.StateStack, self.State)
	if self.State.OnPushed then
		self.State:OnPushed()
	end
	self:_SetState(state:new({
		machine = self,
		EnterState = state.EnterState or EnterState,
		PushState  = state.PushState  or PushState,
		PopState   = state.PopState   or PopState
	}))
	if self.State.OnEnter then
		self.State:OnEnter(...)
	end
end

function StateMachine:PopState(...)
	if self.State.OnExit then
		self.State:OnExit()
	end
	self:_SetState(table.remove(self.StateStack))
	if self.State.OnPopped then
		self.State:OnPopped(...)
	end
end

function StateMachine:_SetState(state)
	self.State = state
end

function StateMachine:Update(dt)
	return self.State:Update(dt)
end

function StateMachine:Render(dt)
	self.State:Render(dt)
end
