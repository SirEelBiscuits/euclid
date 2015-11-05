#include "Textures.h"

namespace Rendering {

	unsigned Texture::PixelsPerMeter = 32;

	Color & Texture::pixel(unsigned x, unsigned y) {
		return pixels[(x & wMask) * h + (y & hMask)];
	}

	Color const & Texture::pixel(unsigned x, unsigned y) const {
		return pixels[(x & wMask) * h + (y & hMask)];
	}

	Color interp(Color s, Color e, float alpha) {
		auto ret = Color{
			(uint8_t)(s.r + (e.r - s.r) * alpha),
			(uint8_t)(s.g + (e.g - s.g) * alpha),
			(uint8_t)(s.b + (e.b - s.b) * alpha),
			(uint8_t)(s.a + (e.a - s.a) * alpha),
		};
		ASSERT(
			(ret.g >= s.g && ret.g <= e.g)
			|| (ret.g <= s.g && ret.g >= e.g)
		);
		return ret;
	}

	Color Texture::pixel_bilinear(btStorageType x, btStorageType y) const {
		int xI = (x < 0 ? x - 1 : x), yI = (y < 0 ? y - 1 : y);
		float xAlpha = x - xI, yAlpha = y - yI;
		auto p00 = pixel(x, y);
		auto p10 = pixel(x + 1, y);
		auto p01 = pixel(x, y + 1);
		auto p11 = pixel(x + 1, y + 1);

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
