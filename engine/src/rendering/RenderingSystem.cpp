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

namespace Rendering 
{ // namespace highlighting is broken if this is put on the correct line

	Context::Context(unsigned Width, unsigned Height, unsigned Scale, bool shouldAllocateScreenBuffer)
		: Width(Width)
		, Height(Height)
		, Scale(Scale)
		, screen{ nullptr }
	{
	}

	Context::~Context()	{
	}
		
	Color & Context::ScreenPixel_slow(unsigned x, unsigned y) {
		if(x < 0 || x >= GetWidth() || y < 0 || y >= GetHeight())
			return Dummy;
		return ScreenPixel(x, y);
	}
		
	btStorageType & Context::DepthPixel_slow(unsigned x, unsigned y) {
		if(x < 0 || x >= GetWidth() || y < 0 || y >= GetHeight())
			return DummyDepth;
		return DepthPixel(x, y);
	}

	void Context::DrawVLine(unsigned x, unsigned yTop, unsigned yBottom, Color c) {
		for(auto y = yTop; y <= yBottom; ++y)
			ScreenPixel(x, y) = c;

		DEBUG_RENDERING();
	}

	void Context::DrawHLine(unsigned xLeft, unsigned xRight, unsigned y, Color c, bool useAlpha /* = false */) {
		if(useAlpha) {
			/*
				auto interpolant = Maths::reverseInterp(0.0f, 255, c.a);
				c.r = ((uint8_t)Maths::interp(dst.r, c.r, interpolant) * m) >> bitShift;
				*/
			auto interpolant = Maths::reverseInterp(0.0f, 255.f, c.a);
			for(auto x = xLeft; x <= xRight; ++x) {
				auto &dst = ScreenPixel(x, y);
				dst.r = (uint8_t)Maths::interp(dst.r, c.r, interpolant);
				dst.g = (uint8_t)Maths::interp(dst.g, c.g, interpolant);
				dst.b = (uint8_t)Maths::interp(dst.b, c.b, interpolant);
			}
		} else {
			for(auto x = xLeft; x <= xRight; ++x)
				ScreenPixel(x, y) = c;
		}


		DEBUG_RENDERING();
	}
	
	void Context::DrawLine(ScreenVec2 start, ScreenVec2 end, Color c) {
		auto Width = (int)GetWidth()-1;
		auto Height = (int)GetHeight()-1;
		if(start.x == end.x)
			if(start.x < 0 || start.x > Width)
				return;
			else
				DrawVLine(start.x,
					Maths::clamp(start.y, 0, Height),
					Maths::clamp(end.y, 0, Height),
					c
				);
		if(start.y == end.y)
			if(start.y < 0 || start.y > Height)
				return;
			else
				DrawHLine(
					Maths::clamp(start.x, 0, Width),
					Maths::clamp(end.x, 0, Width),
					start.y,
					c
				);

		if(start.x > end.x)
			std::swap(start, end);
		if(end.x < 0 || start.x > Width)
			return;
		
		//todo make fixed point

		if(start.x < 0) {
			float m = -(float)start.x / (end.x - start.x);
			start.y += (int)std::round(m * (end.y - start.y));
			start.x = 0;
		}
		if(end.x > Width) {
			float m = ((float)Width - start.x) / (end.x - start.x);
			end.y = start.y + (int)std::round(m * (end.y - start.y));
			end.x = Width;
		}

		if(start.y < 0) {
			if(end.y < 0)
				return;
			float m = -(float)start.y / (end.y - start.y);
			start.x += (int)std::round(m * (end.x - start.x));
			start.y = 0;
		} else if(start.y > Height) {
			if(end.y > Height)
				return;
			float m = ((float)Height  - end.y) / (start.y - end.y);
			start.x = end.x + (int)std::round(m * (start.x - end.x));
			start.y = Height;
		}

		if(end.y < 0) {
			float m = -(float)end.y / (start.y - end.y);
			end.x += (int)std::round(m * (start.x - end.x));
			end.y = 0;
		} else if(end.y > Height) {
			float m = ((float)Height - start.y) / (end.y - start.y);
			end.x = start.x + (int)std::round(m * (end.x - start.x));
			end.y = Height;
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

	void Context::DrawRect(ScreenRect rect, Color c, bool useAlpha /* = false */ ) {
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

		if(rect.size.x == 0 || rect.size.y == 0)
			return;

		auto left = rect.pos.x;
		auto right = Maths::min(GetWidth(), static_cast<unsigned>(left + rect.size.x - 1));
		unsigned bottom = rect.pos.y + rect.size.y;
		for(unsigned y = rect.pos.y; y < bottom && y < GetHeight(); ++y)
			DrawHLine(left, right, y, c, useAlpha);
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
				DrawRectAlpha(dstR, tex, srcR, 1, 0);
				curPos.x += size.x;
			}
		}
	}

	void Context::DrawHLine(
		unsigned xLeft, unsigned xRight,
		unsigned y,
		Texture const *tex,
		UVVec2 start, UVVec2 end,
		btStorageType colorMult,
		btStorageType depth
	) {
		auto xLen = static_cast<Fix16>(xRight - xLeft);
		auto deltaUV = (xLen == 0_fp ? UVVec2(0_fp,0_fp) : (end - start) / xLen);
		auto colMulF16 = Fix16(colorMult);
	
		auto uvCur = start;
		auto idx = y * Width + xLeft;
		for(auto x = xLeft; x <= xRight; ++x, uvCur += deltaUV, idx++) {
#ifdef BILINEAR_FILTERING
			ScreenPixel(x, y) = tex->pixel_bilinear(
				uvCur.x,
				uvCur.y
			) * colMulF16;
#else
			auto texPix = tex->pixel(
				(unsigned)uvCur.x,
				(unsigned)uvCur.y
			);
			texPix = texPix * colMulF16;
			screen[idx] = texPix;
#endif
			this->depth[idx] = depth;
		}

		DEBUG_RENDERING();
	}

	void Context::DrawRect(
		ScreenRect dest,
		Texture const *tex,
		UVRect src,
		btStorageType colorMult,
		btStorageType depth
	) {
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

		for(; x < xTarget && static_cast<unsigned>(x) < GetWidth(); x +=1, ax += dx) {
			//handle dest.pos starting beyond the top of the screen
			auto ay = src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= Fix16(dest.pos.y) * dy;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < GetHeight(); y += 1, ay += dy) {
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
				DepthPixel(x, y) = depth;
			}

			DEBUG_RENDERING();
		}
	}

	void Context::DrawRectAlpha(
		ScreenRect dest,
		Texture const * tex,
		UVRect src,
		btStorageType colorMult,
		btStorageType depth
	) {
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

		for(; x < xTarget && static_cast<unsigned>(x) < GetWidth(); x +=1, ax += dx) {
			//handle dest.pos starting beyond the top of the screen
			auto ay = src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= Fix16(dest.pos.y) * dx;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < GetHeight(); y += 1, ay += dy) {
				//todo - get rid of the floating point maths
#ifdef BILINEAR_FILTERING
				Color c = tex->pixel_bilinear(ax, ay);
#else
				Color c = tex->pixel((unsigned)ax, (unsigned)ay);
#endif
				if(c.a == 0)
					continue;
				Color dst = ScreenPixel(x, y);
				auto interpolant = Maths::reverseInterp(0.0f, 255, c.a);
				c.r = ((uint8_t)Maths::interp(dst.r, c.r, interpolant) * m) >> bitShift;
				c.g = ((uint8_t)Maths::interp(dst.g, c.g, interpolant) * m) >> bitShift;
				c.b = ((uint8_t)Maths::interp(dst.b, c.b, interpolant) * m) >> bitShift;
				ScreenPixel(x, y) = c;
				DepthPixel(x, y) = depth;
			}

			DEBUG_RENDERING();
		}
	}

	void Context::DrawRectAlphaDepth(
		ScreenRect dest,
		Texture const *tex,
		UVRect src,
		btStorageType colorMult,
		btStorageType depth
	) {
		//todo: use actual fixed point type?
		auto const bitShift = 16u;
		auto const bitmult = 1 << bitShift;

		auto const dx = src.size.x / Fix16(dest.size.x);
		auto const dy = src.size.y / Fix16(dest.size.y);
		auto const xTarget = dest.pos.x + dest.size.x;
		auto const yTarget = dest.pos.y + dest.size.y;
		auto ax = src.pos.x;

		auto const m = static_cast<unsigned>(bitmult);

		//handle dest.pos starting beyond the left of the screen
		auto x = dest.pos.x;
		if(dest.pos.x < 0) {
			ax -= Fix16(dest.pos.x) * dx;
			x = 0;
		}

		for(; x < xTarget && static_cast<unsigned>(x) < GetWidth(); x +=1, ax += dx) {

			//handle dest.pos starting beyond the top of the screen
			auto ay = src.pos.y;
			auto y = 0;
			if(dest.pos.y < 0) {
				ay -= Fix16(dest.pos.y) * dx;
			} else {
				y = dest.pos.y;
			}

			for(; y < yTarget && static_cast<unsigned>(y) < GetHeight(); y += 1, ay += dy) {
				//todo - get rid of the floating point maths
				Color dst = ScreenPixel(x, y);
#ifdef BILINEAR_FILTERING
				Color c = tex->pixel_bilinear(ax, ay);
#else
				Color c = tex->pixel((unsigned)ax, (unsigned)ay);
#endif
				auto interpolant = Maths::reverseInterp(0.0f, 255, c.a);
				c.r = ((uint8_t)Maths::interp(dst.r, c.r * colorMult, interpolant) * bitmult) >> bitShift;
				c.g = ((uint8_t)Maths::interp(dst.g, c.g * colorMult, interpolant) * bitmult) >> bitShift;
				c.b = ((uint8_t)Maths::interp(dst.b, c.b * colorMult, interpolant) * bitmult) >> bitShift;
				auto &oldDepth = DepthPixel(x, y);
				ScreenPixel(x, y) = depth <= oldDepth ? (c.a > 0 ? c : dst) : dst;
				oldDepth = depth < oldDepth ? (c.a > 0 ? depth : oldDepth) : oldDepth;
			}

			DEBUG_RENDERING();
		}
	}
}
