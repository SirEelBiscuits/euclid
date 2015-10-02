#include "RenderingSystem.h"

namespace Rendering {

	Context::Context(unsigned Width, unsigned Height, bool shouldAllocateScreenBuffer)
		: Width(Width)
		, Height(Height)
		, screen{ nullptr }
	{
	}

	Context::~Context()	{
	}

}