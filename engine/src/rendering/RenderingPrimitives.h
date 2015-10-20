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
	Color operator*(Color c, float s);

	template<typename StorageType>
	struct Rect {
		Rect() : pos(), size() {}
		Rect(StorageType pos, StorageType size) : pos(pos), size(size) {}
		Rect(int x, int y, int w, int h) : pos(x, y), size(w, h) {}

		StorageType pos;
		StorageType size;
	};

	using ScreenRect = Rect<ScreenVec2>;
	using UVRect = Rect<UVVec2>;
}
