return {
	{
		Name = "Quit",
		Key = 27
	},
	{
		Name = "reinit",
		Key = "r"
	},
	{
		Name = "OpenMap",
		Key = "o",
		CtrlPressed = true
	},
	{
		Name = "Preview",
		Key = "p"
	},
	{
		Name = "Turn",
		MouseAxis = "x",
		Relative = true,
		Scale = -0.1
	},
	{
		Name = "PreviewForward",
		PosKey = "w",
		NegKey = "s",
	},
	{
		Name = "PreviewRight",
		PosKey = "d",
		NegKey = "a",
	},
	{
		Name = "PreviewRaiseCeiling",
		Key = "c",
		ShiftPressed = false
	},
	{
		Name = "PreviewLowerCeiling",
		Key = "c",
		ShiftPressed = true
	},
	{
		Name = "PreviewRaiseFloor",
		Key = "f",
		ShiftPressed = false,
		CtrlPressed = true
	},
	{
		Name = "PreviewLowerFloor",
		Key = "f",
		ShiftPressed = true,
		CtrlPressed = true
	},
	{
		Name = "RaiseCamera",
		PosKey = "q",
		NegKey = "e"
	},
	{
		Name = "Save",
		Key = "s",
		CtrlPressed = true
	},
	{
		Name = "AddSelect",
		MouseButton = "Left",
		ShiftPressed = true
	},
	{
		Name = "ExclusiveSelect",
		MouseButton = "Left",
		ShiftPressed = false
	},
	{
		Name = "DragObject",
		MouseButton = "Middle"
	},
	{
		Name = "DragCamera",
		MouseButton = "Right",
	},
	{
		Name = "DeleteObject",
		Key = 127 -- delete key
	},
	{
		Name = "SplitWall",
		Key = "s"
	},
	{
		Name = "JoinSectors",
		Key = "j"
	},
	{
		Name = "EnterDrawSectorMode",
		Key = "n"
	},
	{
		Name = "DrawSectorPlaceVert",
		MouseButton = "Left",
	},
	{
		Name = "DrawSectorUnplaceVert",
		MouseButton = "Right"
	},
	{
		Name = "DrawSectorModeDone",
		Key = 13
	},
	{
		Name = "DrawSectorModeCancel",
		Key = 27
	},
	{
		Name = "SetCeilHeight",
		Key = "c",
		ShiftPressed = false
	},
	{
		Name = "SetFloorHeight",
		Key = "f",
		ShiftPressed = false
	},
	{
		Name = "SetLightLevel",
		Key = "l"
	},
	{
		Name = "SetCeilTexture",
		Key = "c",
		ShiftPressed = true
	},
	{
		Name = "SetFloorTexture",
		Key = "f",
		ShiftPressed = true
	},
	{
		Name = "SetTopTexture",
		Key = "t"
	},
	{
		Name = "SetMainTexture",
		Key = "m"
	},
	{
		Name = "SetBottomTexture",
		Key = "b"
	},
	{
		Name = "CancelTexturePick",
		Key = 27
	},
	{
		Name = "TexturePickerAddNew",
		Key = "n"
	},
	{
		Name = "TexturePickerUp",
		Key = 1073741906
	},
	{
		Name = "TexturePickerDown",
		Key = 1073741905
	},
	{
		Name = "TexturePickerLeft",
		Key = 1073741904
	},
	{
		Name = "TexturePickerRight",
		Key = 1073741903
	},
	{
		Name = "TexturePickerPageUp",
		Key = 1073741899
	},
	{
		Name = "TexturePickerPageDown",
		Key = 1073741902
	},
	{
		Name = "TexturePickerSelect",
		Key = 13
	},

	-- Mouse Input

	{
		Name = "MouseX",
		MouseAxis = "x",
		Relative = false
	},
	{
		Name = "MouseXRel",
		MouseAxis = "x",
		Relative = true,
		Scale = 1
	},
	{
		Name = "MouseY",
		MouseAxis = "y",
		Relative = false
	},
	{
		Name = "MouseYRel",
		MouseAxis = "y",
		Relative = true,
		Scale = 1
	},
	{
		Name = "LClick",
		MouseButton = "Left"
	},
	{
		Name = "RClick",
		MouseButton = "Right"
	},
	{
		Name = "MClick",
		MouseButton = "Middle"
	},

	-- Undo

	{
		Name = "Undo",
		Key = "z",
		CtrlPressed = true,
		ShiftPressed = false,
	},
	{
		Name = "Redo",
		Key = "z",
		CtrlPressed = true,
		ShiftPressed = true,
	},
}

