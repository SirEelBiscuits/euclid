#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cinttypes>
#include <memory>
POST_STD_LIB

namespace Rendering {
	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct Context {
		Context(unsigned Width, unsigned Height, bool shouldAllocateScreenBuffer);
		virtual ~Context();

		//inline because they need to be fast fast fast!
		unsigned  GetWidth()                          { return Width; };
		unsigned  GetHeight()                         { return Height; };
		Color    &ScreenPixel(unsigned x, unsigned y) { return screen[x + Width * y]; };

		virtual void Clear(Color c) = 0;
		virtual void SetResolution(unsigned Width, unsigned Height) = 0;
		virtual void FlipBuffers() = 0;


	protected:
		Color*   screen{nullptr};
		unsigned Height{0u};
		unsigned Width {0u};
	};

	std::unique_ptr<Context> GetContext(
		unsigned Width,
		unsigned Height,
		unsigned Scale,
		bool FullScreen,
		char const *Title
	);
}