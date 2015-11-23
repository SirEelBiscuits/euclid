return {
	{
		Name = "Quit",
		ControlType = "button",
		Key = 27
	},
	{
		Name = "reinit",
		ControlType = "button",
		Key = "r"
	},
	{
		Name = "OpenMap",
		ControlType = "button",
		Key = "o",
		CtrlPressed = true
	},
	{
		Name = "Preview",
		ControlType = "button",
		Key = "p"
	},
	{
		Name = "Turn",
		ControlType = "mouse",
		axis = "x",
		relative = true,
		scale = -0.1
	},
	{
		Name = "RaiseCeiling",
		ControlType = "button",
		Key = "d"
	},
	{
		Name = "LowerCeiling",
		ControlType = "button",
		Key = "c"
	},
	{
		Name = "RaiseFloor",
		ControlType = "button",
		Key = "f"
	},
	{
		Name = "LowerFloor",
		ControlType = "button",
		Key = "v"
	},
	{
		Name = "Save",
		ControlType = "button",
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
		Name = "DragCamera",
		ControlType = "button",
		MouseButton = "right",
	},
	{
		Name = "DeleteObject",
		ControlType = "button",
		Key = 127 -- delete key
	},
	{
		Name = "SplitWall",
		ControlType = "button",
		Key = "s"
	},
	{
		Name = "JoinSectors",
		ControlType = "button",
		Key = "j"
	},
	{
		Name = "EnterDrawSectorMode",
		ControlType = "button",
		Key = "n"
	},
	{
		Name = "DrawSectorPlaceVert",
		ControlType = "button",
		MouseButton = "left",
	},
	{
		Name = "DrawSectorUnplaceVert",
		ControlType = "button",
		MouseButton = "right"
	},
	{
		Name = "DrawSectorModeDone",
		ControlType = "button",
		Key = 13
	},
	{
		Name = "DrawSectorModeCancel",
		ControlType = "button",
		Key = 27
	},
	{
		Name = "SetCeilHeight",
		ControlType = "button",
		Key = "c",
		ShiftPressed = false
	},
	{
		Name = "SetFloorHeight",
		ControlType = "button",
		Key = "f",
		ShiftPressed = false
	},
	{
		Name = "SetCeilTexture",
		ControlType = "button",
		Key = "c",
		ShiftPressed = true
	},
	{
		Name = "SetFloorTexture",
		ControlType = "button",
		Key = "f",
		ShiftPressed = true
	},
	{
		Name = "SetTopTexture",
		ControlType = "button",
		Key = "t"
	},
	{
		Name = "SetMainTexture",
		ControlType = "button",
		Key = "m"
	},
	{
		Name = "SetBottomTexture",
		ControlType = "button",
		Key = "b"
	},
	{
		Name = "CancelTexturePick",
		ControlType = "button",
		Key = 27
	},
	{
		Name = "TexturePickerAddNew",
		ControlType = "button",
		Key = "n"
	},
	{
		Name = "TexturePickerUp",
		ControlType = "button",
		Key = 1073741906
	},
	{
		Name = "TexturePickerDown",
		ControlType = "button",
		Key = 1073741905
	},
	{
		Name = "TexturePickerLeft",
		ControlType = "button",
		Key = 1073741904
	},
	{
		Name = "TexturePickerRight",
		ControlType = "button",
		Key = 1073741903
	},
	{
		Name = "TexturePickerPageUp",
		ControlType = "button",
		Key = 1073741899
	},
	{
		Name = "TexturePickerPageDown",
		ControlType = "button",
		Key = 1073741902
	},
	{
		Name = "TexturePickerSelect",
		ControlType = "button",
		Key = 13
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

	-- Undo
	
	{
		Name = "Undo",
		ControlType = "button",
		Key = "z",
		CtrlPressed = true,
		ShiftPressed = false,
	},
	{
		Name = "Redo",
		ControlType = "button",
		Key = "z",
		CtrlPressed = true,
		ShiftPressed = true,
	},
}

