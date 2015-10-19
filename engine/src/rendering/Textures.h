#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
#include <map>
POST_STD_LIB

#include "RenderingPrimitives.h"

namespace Rendering {
	class Texture {
	public:
		Texture(unsigned w, unsigned h)
			: w(w)
			, h(h)
			, wMask(w - 1)
			, hMask(h - 1)
		{
			//TODO on error at runtime?
			ASSERT((w & wMask) == 0);
			ASSERT((h & hMask) == 0);

			pixels = new Color[w * h];
		}

		unsigned const w, h;
		unsigned const wMask, hMask;

		Color & pixel(unsigned x, unsigned y);
		Color const & pixel(unsigned x, unsigned y) const;

	private:
		Color *pixels;
	};

	namespace TextureStore {
		using TextureRef = std::shared_ptr<Texture>;

		TextureRef LoadTexture(char const *filename);
		TextureRef GetTexture(char const *filename);
		void ClearUnique();
	}

}