return {
  verts = {
    {
      x = 2,
      y = 2,
    },
    {
      x = -2,
      y = 2,
    },
    {
      x = -2,
      y = -2,
    },
    {
      x = 2,
      y = -2,
    },
    {
      x = 0,
      y = 3,
    },
    {
      z = 0.0,
      x = 9.075,
      y = 1.95,
    },
    {
      z = 0.0,
      x = 9.075,
      y = -5.825,
    },
    {
      z = 0.0,
      x = -2.0,
      y = -8.675,
    },
    {
      z = 0.0,
      x = -12.025,
      y = -8.65,
    },
    {
      z = 0.0,
      x = -12.1,
      y = 9.0,
    },
    {
      z = 0.0,
      x = -2.65,
      y = 8.825,
    },
  },
  sectors = {
    {
      ceilTex = {
        tex = "ceil.png",
      },
      lightLevel = 0.5,
      ceilHeight = 2.25,
      centroid = {
        z = 1.65,
        x = 0.0,
        y = 0.0,
      },
      floorHeight = 0,
      floorTex = {
        tex = "floor.png",
      },
      walls = {
        {
          start = 1,
          portal = 2,
          topTex = {
            tex = "wall.png",
          },
          bottomTex = {
            tex = "wall.png",
          },
          mainTex = {
            tex = "curtain.png",
          },
        },
        {
          topTex = {
            tex = "wall.png",
          },
          start = 2,
          portal = 4,
          mainTex = {
            tex = "",
          },
        },
        {
          start = 3,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 4,
          portal = 3,
          mainTex = {
            tex = "",
          },
        },
      },
    },
    {
      ceilTex = {
        tex = "ceil.png",
      },
      floorTex = {
        tex = "floor.png",
      },
      ceilHeight = 2.0,
      centroid = {
        z = 0.0,
        x = 0.0,
        y = 2.3333333333333,
      },
      floorHeight = 1,
      lightLevel = 0.9,
      walls = {
        {
          start = 1,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 5,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          portal = 1,
          start = 2,
          topTex = {
            tex = "wall.png",
          },
          bottomTex = {
            tex = "wall.png",
          },
        },
      },
    },
    {
      ceilTex = {
        tex = "ceil.png",
      },
      centroid = {
        z = 0.0,
        x = 5.5375,
        y = -0.96875,
      },
      ceilHeight = 2.25,
      floorTex = {
        tex = "floor.png",
      },
      floorHeight = 0,
      lightLevel = 0.2,
      walls = {
        {
          start = 1,
          portal = 1,
          mainTex = {
            tex = "",
          },
        },
        {
          start = 4,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 7,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 6,
          mainTex = {
            tex = "wall.png",
          },
        },
      },
    },
    {
      centroid = {
        z = 0.0,
        x = -5.4625,
        y = 0.083333333333333,
      },
      ceilHeight = 2.5,
      lightLevel = 0.95,
      floorHeight = 0,
      floorTex = {
        tex = "floor.png",
      },
      walls = {
        {
          topTex = {
            tex = "wall.png",
          },
          start = 3,
          mainTex = {
            tex = "",
          },
          portal = 1,
        },
        {
          start = 2,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 11,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 10,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 9,
          mainTex = {
            tex = "wall.png",
          },
        },
        {
          start = 8,
          mainTex = {
            tex = "wall.png",
          },
        },
      },
    },
  },
}
