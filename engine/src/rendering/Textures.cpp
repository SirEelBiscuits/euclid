#include "Textures.h"

namespace Rendering {

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

		void ClearUnique() {
			for(auto &kvp : TexLookup)
				if(kvp.second.unique())
					kvp.second.reset();
		}
	}
}
