#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cinttypes>
POST_STD_LIB

#include "lib/BasicTypes.h"

namespace Rendering {
	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	struct Rect {
		Rect() : pos(), size() {}
		Rect(ScreenVec2 pos, ScreenVec2 size) : pos(pos), size(size) {}
		Rect(int x, int y, int w, int h) : pos(x, y), size(w, h) {}

		ScreenVec2 pos;
		ScreenVec2 size;
	};
}
