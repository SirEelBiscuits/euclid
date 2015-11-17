return {
	{
		ControlType = "button",
		Name = "Quit",
		Key = 27
	},
	{
		ControlType = "button",
		Name = "reinit",
		Key = "r"
	},
	{
		ControlType = "button",
		Name = "OpenMap",
		Key = "o"
	},
	{
		ControlType = "button",
		Name = "Preview",
		Key = "p"
	},
	{
		ControlType = "mouse",
		Name = "Turn",
		axis = "x",
		relative = true,
		scale = -0.1
	},
	{
		ControlType = "button",
		Name = "RaiseCeiling",
		Key = 127 -- delete key
	},
	{
		ControlType = "button",
		Name = "LowerCeiling",
		Key = "c"
	},
	{
		ControlType = "button",
		Name = "RaiseFloor",
		Key = "f"
	},
	{
		ControlType = "button",
		Name = "LowerFloor",
		Key = "v"
	},
	{
		ControlType = "button",
		Name = "Save",
		Key = "s",
		CtrlPressed = true
	},
	{
		Name = "AddSelect",
		ControlType = "button",
		MouseButton = "left",
		ShiftPressed = true
	},
	{
		Name = "ExclusiveSelect",
		ControlType = "button",
		MouseButton = "left",
		ShiftPressed = false
	},
	{
		Name = "DragObject",
		ControlType = "button",
		MouseButton = "middle"
	},
	{
		Name = "DeleteObject",
		ControlType = "button",
		Key = "d"
	},


	-- Mouse Input

	{
		Name = "MouseX",
		ControlType = "mouse",
		axis = "x",
		relative = false
	},
	{
		Name = "MouseXRel",
		ControlType = "mouse",
		axis = "x",
		relative = true,
		scale = 1
	},
	{ 
		Name = "MouseY",
		ControlType = "mouse",
		axis = "y",
		relative = false
	},
	{ 
		Name = "MouseYRel",
		ControlType = "mouse",
		axis = "y",
		relative = true,
		scale = 1
	},
	{
		Name = "LClick",
		ControlType = "button",
		MouseButton = "left"
	},
	{
		Name = "RClick",
		ControlType = "button",
		MouseButton = "right"
	},
	{
		Name = "MClick",
		ControlType = "button",
		MouseButton = "middle"
	},
}

