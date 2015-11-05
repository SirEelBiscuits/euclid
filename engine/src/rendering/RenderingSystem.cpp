#include "RenderingSystem.h"

#include "lib/Maths.h"

#ifdef EUCLID_DEBUG
#	include "system/Events.h"
#	define DEBUG_RENDERING() \
	do { \
		if(debugRendering) { \
			FlipBuffers();\
			debugRendering &= !System::Input::ReturnOnKeyInput();\
		} \
	 } while(false)
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

		if(start.x == end.x)
			if(start.x < 0 || start.x >= Width)
				return;
			else
				DrawVLine(start.x,
					Maths::clamp(start.y, 0, (int)GetWidth()-1),
					Maths::clamp(end.y, 0, (int)GetHeight()-1),
					c
				);
		if(start.y == end.y)
			if(start.y < 0 || start.y >= Height)
				return;
			else
				DrawHLine(
					Maths::clamp(start.x, 0, (int)GetWidth()-1),
					Maths::clamp(end.x, 0, (int)GetWidth()-1),
					start.y,
					c
				);

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

		// Bresenham's algorithm, see:
		// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

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
		auto right = Maths::min(Width, static_cast<unsigned>(left + rect.size.x));
		unsigned bottom = rect.pos.y + rect.size.y;
		for(unsigned y = rect.pos.y; y < bottom && y < Height; ++y)
			DrawHLine(left, right, y, c);
	}

	void Context::DrawRect(ScreenVec2 topLeft, ScreenVec2 topRight, Color c) {
		DrawRect(Rendering::ScreenRect(topLeft, topRight - topLeft), c);
	}

	void Context::DrawText(ScreenVec2 topLeft, Texture * tex, char const * text) {
		if(tex == nullptr)
			return;
		auto const size = ScreenVec2((int)tex->w / 32, (int)tex->h / 8);
		auto curPos = topLeft;
		for(auto curChar = text; *curChar != 0; ++curChar) {
			switch(*curChar) {
			case '\n':
				topLeft.y += size.y;
				curPos = topLeft;
				break;
			default:
				auto dstR = ScreenRect(curPos, size);
				auto srcR = UVRect(
					UVVec2(
						Fix16(*curChar % 32 * size.x),
						Fix16(*curChar / 32 * size.y)
					),
					UVVec2(
						Fix16(size.x),
						Fix16(size.y)
					)
				);
				DrawRectAlpha(dstR, tex, srcR, 1);
				curPos.x += size.x;
			}
		}
	}

	void Context::DrawHLine(
		unsigned xLeft, unsigned xRight,
		unsigned y,
		Texture const *tex,
		UVVec2 start, UVVec2 end,
		btStorageType colorMult
	) {
		auto xLen = static_cast<Fix16>(xRight - xLeft);
		auto deltaUV = (xLen == 0_fp ? UVVec2(0_fp,0_fp) : (end - start) / xLen);
	
		auto uvCur = start;
		for(auto x = xLeft; x <= xRight; ++x) {
#ifdef BILINEAR_FILTERING
			ScreenPixel(x, y) = tex->pixel_bilinear(
				uvCur.x,
				uvCur.y
			) * colorMult;
#else
			ScreenPixel(x, y) = tex->pixel(
				(unsigned)uvCur.x,
				(unsigned)uvCur.y
			) * colorMult;
#endif
			uvCur += deltaUV;
		}

		DEBUG_RENDERING();
	}

	void Context::DrawRect(ScreenRect dest, Texture const *tex, UVRect src, float colorMult) {
		//todo: use actual fixed point type?
		auto const bitShift = 16u;
		auto const bitmult = 1 << bitShift;

		auto const dx = src.size.x / Fix16(dest.size.x);
		auto const dy = src.size.y / Fix16(dest.size.y);
		auto const xTarget = dest.pos.x + dest.size.x;
		auto const yTarget = dest.pos.y + dest.size.y;
		auto ax = src.pos.x;

		auto const m = static_cast<Fix16>(colorMult);

		//handle dest.pos starting beyond the left of the screen
		auto x = 0;
		if(dest.pos.x < 0) {
			ax -= Fix16(dest.pos.x) * dx;
		} else {
			x = dest.pos.x;
		}

		for(; x < xTarget && static_cast<unsigned>(x) < Width; x +=1, ax += dx) {
			//handle dest.pos starting beyond the top of the screen
			auto ay = src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= Fix16(dest.pos.y) * dy;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < Height; y += 1, ay += dy) {
				//todo: is this actually faster than float multiplication..?
#ifdef BILINEAR_FILTERING
				Color c = tex->pixel_bilinear(ax, ay);
#else
				Color c = tex->pixel((unsigned)ax, (unsigned)ay);
#endif
				c.r = (uint8_t)((Fix16)c.r * m);
				c.g = (uint8_t)((Fix16)c.g * m);
				c.b = (uint8_t)((Fix16)c.b * m);
				ScreenPixel(x, y) = c;
			}

			DEBUG_RENDERING();
		}
	}

	void Context::DrawRectAlpha(ScreenRect dest, Texture const * tex, UVRect src, float colorMult) {
		//todo: use actual fixed point type?
		auto const bitShift = 16u;
		auto const bitmult = 1 << bitShift;

		auto const dx = src.size.x / Fix16(dest.size.x);
		auto const dy = src.size.y / Fix16(dest.size.y);
		auto const xTarget = dest.pos.x + dest.size.x;
		auto const yTarget = dest.pos.y + dest.size.y;
		auto ax = src.pos.x;

		auto const m = static_cast<unsigned>(bitmult * colorMult);

		//handle dest.pos starting beyond the left of the screen
		auto x = 0;
		if(dest.pos.x < 0) {
			ax -= Fix16(dest.pos.x) * dx;
		} else {
			x = dest.pos.x;
		}

		for(; x < xTarget && static_cast<unsigned>(x) < Width; x +=1, ax += dx) {
			//handle dest.pos starting beyond the top of the screen
			auto ay = src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= Fix16(dest.pos.y) * dx;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < Height; y += 1, ay += dy) {
				//todo - get rid of the floating point maths
				Color dst = ScreenPixel(x, y);
#ifdef BILINEAR_FILTERING
				Color c = tex->pixel_bilinear(ax, ay);
#else
				Color c = tex->pixel((unsigned)ax, (unsigned)ay);
#endif
				auto interpolant = Maths::reverseInterp(0.0f, 255, c.a);
				c.r = ((uint8_t)Maths::interp(dst.r, c.r, interpolant) * m) >> bitShift;
				c.g = ((uint8_t)Maths::interp(dst.g, c.g, interpolant) * m) >> bitShift;
				c.b = ((uint8_t)Maths::interp(dst.b, c.b, interpolant) * m) >> bitShift;
				ScreenPixel(x, y) = c;
			}

			DEBUG_RENDERING();
		}
	}
}
