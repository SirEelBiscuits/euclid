Editor.CameraDragState = Editor.CameraDragState or {}

function Editor.CameraDragState:Enter()
	print("entering camera drag state")

	Editor.State = self
end

function Editor.CameraDragState:Update()
	Editor.view.eye.x = Editor.view.eye.x - Game.Controls.MouseXRel / Editor.view.scale
	Editor.view.eye.y = Editor.view.eye.y + Game.Controls.MouseYRel / Editor.view.scale

	if not Game.Controls.DragCamera.isDown then
		Editor.DefaultState:Enter(true)
	end
end

function Editor.CameraDragState:Render()
	Editor.DefaultState:Render()
end
