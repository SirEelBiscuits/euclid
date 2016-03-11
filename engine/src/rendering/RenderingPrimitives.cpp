#include "RenderingPrimitives.h"

namespace Rendering {

	Color operator*(Color c, float s)
	{
		c.r = (uint8_t)(c.r * s);
		c.g = (uint8_t)(c.g * s);
		c.b = (uint8_t)(c.b * s);
		return c;
	}

	Color operator*(Color c, Fix16 s)
	{
		c.r = (uint8_t)(Fix16(c.r) * s);
		c.g = (uint8_t)(Fix16(c.g) * s);
		c.b = (uint8_t)(Fix16(c.b) * s);
		return c;
	}

}
