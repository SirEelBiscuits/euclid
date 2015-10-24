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

		if(start.x == end.x)
			DrawVLine(start.x, start.y, end.y, c);
		if(start.y == end.y)
			DrawHLine(start.x, end.x, start.y, c);

		if(start.x > end.x)
			std::swap(start, end);
		
		//todo make fixed point

		if(start.x < 0) {
			float m = -(float)start.x / (end.x - start.x);
			start.y += (int)std::round(m * (end.y - start.y));
			start.x = 0;
		}
		if(end.x >= (int)Width) {
			float m = ((float)Width - 1 - start.x) / (end.x - start.x);
			end.y = start.y + (int)std::round(m * (end.y - start.y));
			end.x = Width - 1;
		}

		if(start.y < 0) {
			float m = -(float)start.y / (end.y - start.y);
			start.x += (int)std::round(m * (end.x - start.x));
			start.y = 0;
		} else if(start.y >= (int)Height) {
			float m = ((float)Height - 1 - end.y) / (start.y - end.y);
			start.x = end.x + (int)std::round(m * (start.x - end.x));
			start.y = Height - 1;
		}

		if(end.y < 0) {
			float m = -(float)end.y / (start.y - end.y);
			end.x += (int)std::round(m * (start.x - end.x));
			end.y = 0;
		} else if(end.y >= (int)Height) {
			float m = ((float)Height - 1 - start.y) / (end.y - start.y);
			end.x = start.x + (int)std::round(m * (end.x - start.x));
			end.y = Height - 1;
		}

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

	void Context::DrawRect(ScreenRect rect, Color c) {
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

		auto left = rect.pos.x;
		auto right = Maths::min(Height, static_cast<unsigned>(left + rect.size.x));
		unsigned bottom = rect.pos.y + rect.size.y;
		for(unsigned y = rect.pos.y; y < bottom && y < Height; ++y)
			DrawHLine(left, right, y, c);
	}

	void Context::DrawRect(ScreenVec2 topLeft, ScreenVec2 topRight, Color c) {
		DrawRect(Rendering::ScreenRect(topLeft, topRight - topLeft), c);
	}

	void Context::DrawHLine(
		unsigned xLeft, unsigned xRight,
		unsigned y,
		Texture const *tex,
		UVVec2 start, UVVec2 end,
		btStorageType colorMult
	) {
		//todo: use actual fixed point type?
		auto const shift = 16u;

		auto xLen = static_cast<float>(xRight - xLeft);
		auto deltaUV = ScreenVec2{
			static_cast<int>((1 << shift) * ((end.x - start.x) / xLen)),
			static_cast<int>((1 << shift) * ((end.y - start.y) / xLen))
		};
	
		auto uvCur = start * (1 << shift);
		for(auto x = xLeft; x <= xRight; ++x) {
			ScreenPixel(x, y) = tex->pixel((uvCur.x >> shift), (uvCur.y >> shift)) * colorMult;
			uvCur += deltaUV;
		}
	}

	void Context::DrawRect(ScreenRect dest, Texture const *tex, UVRect src, float colorMult) {
		//todo: use actual fixed point type?
		auto const bitShift = 16u;
		auto const bitmult = 1 << bitShift;

		auto const dx = bitmult * src.size.x / dest.size.x;
		auto const dy = bitmult * src.size.y / dest.size.y;
		auto const xTarget = dest.pos.x + dest.size.x;
		auto const yTarget = dest.pos.y + dest.size.y;
		auto ax = bitmult * src.pos.x;

		auto const m = static_cast<unsigned>(bitmult * colorMult);

		//handle dest.pos starting beyond the left of the screen
		auto x = 0;
		if(dest.pos.x < 0) {
			ax -= dest.pos.x * dx;
		} else {
			x = dest.pos.x;
		}

		for(; x < xTarget && static_cast<unsigned>(x) < Width; x +=1, ax += dx) {
			//handle dest.pos starting beyond the top of the screen
			auto ay = bitmult * src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= dest.pos.y * dx;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < Height; y += 1, ay += dy) {
				//todo: is this actually faster than float multiplication..?
				Color c = tex->pixel(ax >> bitShift, ay >> bitShift);
				c.r = (c.r * m) >> bitShift;
				c.g = (c.g * m) >> bitShift;
				c.b = (c.b * m) >> bitShift;
				ScreenPixel(x, y) = c;
			}
		}
	}

	void Context::DrawRectAlpha(ScreenRect dest, Texture const * tex, UVRect src, float colorMult) {
		//todo: use actual fixed point type?
		auto const bitShift = 16u;
		auto const bitmult = 1 << bitShift;

		auto const dx = bitmult * src.size.x / dest.size.x;
		auto const dy = bitmult * src.size.y / dest.size.y;
		auto const xTarget = dest.pos.x + dest.size.x;
		auto const yTarget = dest.pos.y + dest.size.y;
		auto ax = bitmult * src.pos.x;

		auto const m = static_cast<unsigned>(bitmult * colorMult);

		//handle dest.pos starting beyond the left of the screen
		auto x = 0;
		if(dest.pos.x < 0) {
			ax -= dest.pos.x * dx;
		} else {
			x = dest.pos.x;
		}

		for(; x < xTarget && static_cast<unsigned>(x) < Width; x +=1, ax += dx) {
			//handle dest.pos starting beyond the top of the screen
			auto ay = bitmult * src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= dest.pos.y * dx;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < Height; y += 1, ay += dy) {
				//todo - get rid of the floating point maths
				Color dst = ScreenPixel(x, y);
				Color c = tex->pixel(ax>>bitShift, ay>>bitShift);
				auto interpolant = Maths::reverseInterp(0.0f, 255, c.a);
				c.r = ((uint8_t)Maths::interp(dst.r, c.r, interpolant) * m) >> bitShift;
				c.g = ((uint8_t)Maths::interp(dst.g, c.g, interpolant) * m) >> bitShift;
				c.b = ((uint8_t)Maths::interp(dst.b, c.b, interpolant) * m) >> bitShift;
				ScreenPixel(x, y) = c;
			}
		}
	}
}