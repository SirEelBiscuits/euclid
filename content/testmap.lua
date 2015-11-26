return {
	verts = {
		{ x = 2, y = 2},
		{ x = -2, y = 2},
		{ x = -2, y = -2},
		{ x = 2, y = -2},
		{ x = 0, y = 3},
	},
	sectors = {
		{
			walls = { 
				{ start = 1, portal = 2, mainTex = {tex = "curtain.png"} },
				{ start = 2, mainTex = {tex = "wall.png"} },
				{ start = 3 },
				{ start = 4 },
			},
			floorHeight = 0,
			ceilHeight = 2.25,
			lightLevel = 0.5,

			floorTex = {tex = "floor.png"},
			ceilTex  = {tex = "ceil.png"},

		},
		{
			walls = {
				{start = 1, portal = 1},
				{start = 2},
				{start = 5},
			},

			floorTex = {tex = "floor.png"},
			ceilTex  = {tex = "ceil.png"},
			lightLevel = 0.9,

			floorHeight = 1,
			ceilHeight = 2.0,
		}
	}
}
