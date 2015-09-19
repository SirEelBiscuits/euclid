#include "RenderingSystem.h"

namespace Rendering {

	Context::Context(unsigned Width, unsigned Height, bool shouldAllocateScreenBuffer)
		: Width(Width)
		, Height(Height)
		, screen{ nullptr }
	{
	}

	Context::~Context()
	{
	}

	unsigned Context::GetWidth()
	{
		return Width;
	}

	unsigned Context::GetHeight()
	{
		return Height;
	}

	Color &Context::ScreenPixel(unsigned x, unsigned y)
	{
		return screen[x + Width * y];
	}

}