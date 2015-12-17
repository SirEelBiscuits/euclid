#include "RenderingPrimitives.h"

namespace Rendering {

	Color operator*(Color c, float s)
	{
		c.r = (uint8_t)(c.r * s);
		c.g = (uint8_t)(c.g * s);
		c.b = (uint8_t)(c.b * s);
		c.a = (uint8_t)(c.a * s);
		return c;
	}

}
