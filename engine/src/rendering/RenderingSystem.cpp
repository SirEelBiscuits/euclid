#include "RenderingSystem.h"

#include "lib/Maths.h"

#ifdef EUCLID_DEBUG
#	define DEBUG_RENDERING()
#else
#	define DEBUG_RENDERING()
#endif

namespace Rendering {

	Context::Context(unsigned Width, unsigned Height, bool shouldAllocateScreenBuffer)
		: Width(Width)
		, Height(Height)
		, screen{ nullptr }
	{
	}

	Context::~Context()	{
	}
		
	Color & Context::ScreenPixel_slow(unsigned x, unsigned y) {
		if(x < 0 || x >= Width || y < 0 || y >= Height)
			return Dummy;
		return ScreenPixel(x, y);
	}

	void Context::DrawVLine(unsigned x, unsigned yTop, unsigned yBottom, Color c) {
		for(auto y = yTop; y <= yBottom; ++y)
			ScreenPixel(x, y) = c;

		DEBUG_RENDERING();
	}

	void Context::DrawHLine(unsigned xLeft, unsigned xRight, unsigned y, Color c) {
		for(auto x = xLeft; x <= xRight; ++x)
			ScreenPixel(x, y) = c;

		DEBUG_RENDERING();
	}
	
	void Context::DrawLine(ScreenVec2 start, ScreenVec2 end, Color c) {
		// Bresenham's algorithm, see:
		// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

		if(start.x > end.x)
			std::swap(start, end);

		ScreenPixel_slow(start) = c;
		ScreenPixel_slow(end) = c;

		if(end.y > start.y) {
			if(end.x - start.x > end.y - start.y) { //oct 0
				auto d = end - start;
				auto D = 2 * d.y - d.x;
				auto y = start.y;

				for(auto x = start.x + 1; x < end.x; ++x) {
					if(D > 0) {
						++y;
						ScreenPixel_slow(x, y) = c;
						D += 2 * d.y - 2 * d.x;
					} else {
						ScreenPixel_slow(x, y) = c;
						D += 2 * d.y;
					}
				}
			} else { //oct 1
				auto d = ScreenVec2{end.y - start.y, end.x - start.x};
				auto D = 2 * d.y - d.x;
				auto y = start.x;

				for(auto x = start.y + 1; x < end.y; ++x) {
					if(D > 0) {
						++y;
						ScreenPixel_slow(y, x) = c;
						D += 2 * d.y - 2 * d.x;
					} else {
						ScreenPixel_slow(y, x) = c;
						D += 2 * d.y;
					}
				}
			}
		} else {
			if(end.x - start.x > start.y - end.y) { //oct 7
				auto d = ScreenVec2{end.x - start.x, start.y - end.y};
				auto D = 2 * d.y - d.x;
				auto y = -start.y;

				for(auto x = start.x + 1; x < end.x; ++x) {
					if(D > 0) {
						++y;
						ScreenPixel_slow(x, -y) = c;
						D += 2 * d.y - 2 * d.x;
					} else {
						ScreenPixel_slow(x, -y) = c;
						D += 2 * d.y;
					}
				}
			} else { //oct 6
				auto d = ScreenVec2{start.y - end.y, end.x - start.x};
				auto D = 2 * d.y - d.x;
				auto y = start.x;

				for(auto x = -start.y + 1; x < -end.y; ++x) {
					if(D > 0) {
						++y;
						ScreenPixel_slow(y, -x) = c;
						D += 2 * d.y - 2 * d.x;
					} else {
						ScreenPixel_slow(y, -x) = c;
						D += 2 * d.y;
					}
				}
			}
		}

		DEBUG_RENDERING();
	}

	void Context::DrawRect(Rect rect, Color c) {
		if(rect.size.x < 0) {
			rect.pos.x += rect.size.x;
			rect.size.x = -rect.size.x;
		}
		if(rect.size.y < 0) {
			rect.pos.y += rect.size.y;
			rect.size.y = -rect.size.y;
		}
		if(rect.pos.x + rect.size.x < 0 || rect.pos.y + rect.size.y < 0)
			return;

		if(rect.pos.x < 0) {
			rect.size.x += rect.pos.x;
			rect.pos.x = 0;
		}
		if(rect.pos.y < 0) {
			rect.size.y += rect.pos.y;
			rect.pos.y = 0;
		}

		auto top = rect.pos.y;
		auto bottom = Maths::min(Height, static_cast<unsigned>(top + rect.size.y));
		unsigned right = rect.pos.x + rect.size.x;
		for(unsigned x = rect.pos.x; x < right && x < Width; ++x)
			DrawVLine(x, top, bottom, c);
	}

	void Context::DrawRect(ScreenVec2 topLeft, ScreenVec2 topRight, Color c) {
		DrawRect(Rendering::Rect(topLeft, topRight - topLeft), c);
	}
}