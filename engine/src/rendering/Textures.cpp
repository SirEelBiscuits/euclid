#include "Textures.h"

namespace Rendering {

	unsigned Texture::PixelsPerMeter = 32;

	Color & Texture::pixel(unsigned x, unsigned y) {
		return pixels[(x & wMask) * h + (y & hMask)];
	}

	Color const & Texture::pixel(unsigned x, unsigned y) const {
		return pixels[(x & wMask) * h + (y & hMask)];
	}

	Color interp(Color s, Color e, Fix16 alpha) {
		auto ret = Color{
			static_cast<uint8_t>(s.b + static_cast<uint8_t>(static_cast<Fix16>(e.b - s.b) * alpha)),
			static_cast<uint8_t>(s.g + static_cast<uint8_t>(static_cast<Fix16>(e.g - s.g) * alpha)),
			static_cast<uint8_t>(s.r + static_cast<uint8_t>(static_cast<Fix16>(e.r - s.r) * alpha)),
			static_cast<uint8_t>(s.a + static_cast<uint8_t>(static_cast<Fix16>(e.a - s.a) * alpha))
		};
		ASSERT(
			(ret.g >= s.g && ret.g <= e.g)
			|| (ret.g <= s.g && ret.g >= e.g)
		);
		return ret;
	}

	Color Texture::pixel_bilinear(Fix16 x, Fix16 y) const {
		auto xI = static_cast<int32_t>(x);
		auto yI = static_cast<int32_t>(y);

		Fix16 xAlpha = x - static_cast<Fix16>(xI), yAlpha = y - static_cast<Fix16>(yI);
		auto p00 = pixel(static_cast<unsigned>(x),     static_cast<unsigned>(y));
		auto p10 = pixel(static_cast<unsigned>(x) + 1, static_cast<unsigned>(y));
		auto p01 = pixel(static_cast<unsigned>(x),     static_cast<unsigned>(y) + 1);
		auto p11 = pixel(static_cast<unsigned>(x) + 1, static_cast<unsigned>(y) + 1);

		auto p0x = interp(p00, p10, xAlpha);
		auto p1x = interp(p01, p11, xAlpha);

		return interp(p0x, p1x, yAlpha);
	}

	namespace TextureStore {
		static std::map<std::string, TextureRef> TexLookup;

		TextureRef GetTexture(char const *filename) {
			auto &test = TexLookup[filename];
			if(test.get() == nullptr)
				test = LoadTexture(filename);
			return test;
		}

		TextureRef GetTexture(Texture const * texp) {
			for(auto &i : TexLookup) {
				if(i.second.get() == texp)
					return i.second;
			}

			return TextureRef{nullptr};
		}

		std::string GetTextureFilename(Texture const * tex) {
			for(auto &kvp : TexLookup) {
				if(kvp.second.get() == tex)
					return kvp.first;
			}
			return "";
		}

		void ClearUnique() {
			for(auto &kvp : TexLookup)
				if(kvp.second.unique())
					kvp.second.reset();
		}
	}
}
