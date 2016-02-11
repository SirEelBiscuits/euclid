local GeneralControls = {
	{
		Name = "Quit",
		Key = 27
	},
	{
		Name = "reinit",
		Key = "r",
		CtrlPressed = true,
		ShiftPressed = true
	},
	{
		Name = "DragCamera",
		MouseButton = "Right",
		CtrlPressed = false,
	},
	{
		Name = "OpenMap",
		Key = "o",
		CtrlPressed = true,
		ShiftPressed = false,
	},
	{
		Name = "Save",
		Key = "s",
		CtrlPressed = true
	},
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
	{
		Name = "Copy",
		Key = "c",
		CtrlPressed = true,
		ShiftPressed = false
	},
	{
		Name = "Paste",
		Key = "v",
		CtrlPressed = true,
		ShiftPressed = false
	},
	{
		Name = "CopyProperties",
		Key = "c",
		CtrlPressed = true,
		ShiftPressed = true,
	},
	{
		Name = "PasteProperties",
		Key = "v",
		CtrlPressed = true,
		ShiftPressed = true
	},
	{
		Name = "Preview",
		Key = "p",
	},
}

local BasicManipulationControls = {
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
		MouseButton = "Middle",
		CtrlPressed = false,
	},
	{
		Name = "ScaleObject",
		MouseButton = "Right",
		CtrlPressed = true,
	},
	{
		Name = "RotateObject",
		MouseButton = "Middle",
		CtrlPressed = true,
	},
	{
		Name = "DeleteObject",
		Key = 127 -- delete key
	},

}

local DrawSectorControls = {
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
}

local AdvancedManipulationControls = {
	{
		Name = "SplitWall",
		Key = "s",
		CtrlPressed = false,
	},
	{
		Name = "JoinSectors",
		Key = "j"
	},
	{
		Name = "SetCeilHeight",
		Key = "r",
		ShiftPressed = false,
		CtrlPressed  = false
	},
	{
		Name = "SetFloorHeight",
		Key = "f",
		ShiftPressed = false,
		CtrlPressed  = false,
	},
	{
		Name = "AdjustCeilHeight",
		Key = "r",
		ShiftPressed = true,
		CtrlPressed  = false,
	},
	{
		Name = "AdjustFloorHeight",
		Key = "f",
		ShiftPressed = true,
		CtrlPressed  = false,
	},
	{
		Name = "SetLightLevel",
		Key = "l"
	},
	{
		Name = "SetStartSector",
		Key = "g",
		ShiftPressed = false,
		CtrlPressed = false
	},
	{
		Name = "SetGroup",
		Key = "g",
		ShiftPressed = true,
		CtrlPressed = false,
	},
	{
		Name = "SelectGroup",
		Key = "g",
		CtrlPressed = true,
		ShiftPressed = true
	},
	{
		Name = "SetOutdoors",
		Key = "o",
		ShiftPressed = false,
		CtrlPressed = false
	},
	{
		Name = "SetIndoors",
		Key = "o",
		ShiftPressed = true,
		CtrlPressed = false,
	},
	{
		Name = "SetOutdoorLevel",
		Key = "o",
		ShiftPressed = true,
		CtrlPressed = true
	}
}

local SpriteControls = {
	{
		Name = "CreateSprite",
		Key = "e",
		ShiftPressed = false,
	},
	{
		Name = "EditThing",
		Key = "e",
		ShiftPressed = true,
	},
}

local DataEditControls = {
	{
		Name = "DataMoveDown",
		Key = "s",
		ShiftPressed = false,
	},
	{
		Name = "DataMoveUp",
		Key = "w",
	},
	{
		Name = "DataEdit",
		Key = "e",
		ShiftPressed = false
	},
	{
		Name = "DataNewTable",
		Key = "t",
	},
	{
		Name = "DataNewNumber",
		Key = "n",
	},
	{
		Name = "DataNewString",
		Key = "s",
		ShiftPressed = true,
	},
	{
		Name = "DataNewBool",
		Key = "b",
	},
	{
		Name = "DataBack",
		Key = 27,
	},
	{
		Name = "DataDelete",
		Key = 127,
	}
}

local TexturingControls = {
	{
		Name = "SetCeilTexture",
		Key = "r",
		ShiftPressed = false,
		CtrlPressed = true
	},
	{
		Name = "SetFloorTexture",
		Key = "f",
		CtrlPressed = true
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
}

local MouseControls = {
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
	{
		Name = "ZoomIn",
		MouseButton = "ScrollUp",
	},
	{
		Name = "ZoomOut",
		MouseButton = "ScrollDown",
	},
}

local function Concat(a, b, ...)
	if not a then
		return Concat(b, ...)
	end
	if not b then
		return Concat(a, ...)
	end
	for i, v in ipairs(b) do
		a[#a + 1] = v
	end
	if ... then
		return Concat(a, ...)
	end
	return a
end

return Concat(
	GeneralControls,
	MouseControls,
	BasicManipulationControls,
	AdvancedManipulationControls,
	TexturingControls,
	DrawSectorControls,
	SpriteControls,
	DataEditControls
)
