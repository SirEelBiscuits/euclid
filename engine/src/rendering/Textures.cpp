#include "Textures.h"

namespace Rendering {

	unsigned Texture::PixelsPerMeter = 32;

	Color & Texture::pixel(unsigned x, unsigned y) {
		return pixels[(x & wMask) * h + (y & hMask)];
	}

	Color const & Texture::pixel(unsigned x, unsigned y) const {
		return pixels[(x & wMask) * h + (y & hMask)];
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
