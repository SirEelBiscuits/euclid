Editor.CameraDragState = Editor.CameraDragState or {}

function Editor.CameraDragState:Enter()
	print("entering camera drag state")

	Editor.State = self
end

function Editor.CameraDragState:Update()
	local movVec = Maths.Vector:new(-Game.Controls.MouseXRel, Game.Controls.MouseYRel) 
		/ Editor.view.scale / Draw.GetScale()
	Editor.view.eye = Editor.view.eye + movVec

	if not Game.Controls.DragCamera.isDown then
		Editor.DefaultState:Enter(true)
	end
end

function Editor.CameraDragState:Render()
	Editor.DefaultState:Render()
end
