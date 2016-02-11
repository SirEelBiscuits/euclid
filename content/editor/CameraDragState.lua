Editor.CameraDragState = Editor.CameraDragState or
	CreateNewClass("CameraDragState")

function Editor.CameraDragState:OnEnter()
	print("entering camera drag state")
end

function Editor.CameraDragState:Update()
	local editor = self.machine.owner
	local movVec = Maths.Vector:new(-Game.Controls.MouseXRel, Game.Controls.MouseYRel)
		/ editor.view.scale / Draw.GetScale()
	editor.view.eye = editor.view.eye + movVec

	if not Game.Controls.DragCamera.isDown then
		self:PopState()
		return
	end
end

function Editor.CameraDragState:Render()
	Editor.DefaultState.Render(self)
end
