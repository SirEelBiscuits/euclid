#include "RenderingPrimitives.h"

namespace Rendering {

Color operator*(Color c, float s)
{
	c.r *= s;
	c.g *= s;
	c.b *= s;
	c.a *= s;
	return c;
}

}
