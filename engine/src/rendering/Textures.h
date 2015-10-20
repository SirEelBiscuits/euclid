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

			pixels = std::make_unique<Color[]>(w * h);
		}

		unsigned const w, h;
		unsigned const wMask, hMask;

		Color & pixel(unsigned x, unsigned y);
		Color const & pixel(unsigned x, unsigned y) const;

	private:
		std::unique_ptr<Color[]> pixels;
	};

	struct TextureInfo {
		Texture *tex  {nullptr};
		UVVec2 uvStart{0,0};
	};

	namespace TextureStore {
		/**
			The authoritative type of a texture reference
		*/
		using TextureRef = std::shared_ptr<Texture>;

		//This function is to be implemented by platform-specific code
		/**
			Load a texture from disk, and return a reference to it.

			This will always load a new copy, even if it's already loaded.
		 */
		TextureRef LoadTexture(char const *filename);

		/**
			Get the texture with the given file name.

			If the texture is already in memory, a reference to the already-loaded copy will 
			be provided, and that reference will keep the texture loaded.
		*/
		TextureRef GetTexture(char const *filename);

		/**
			Check if any textures are no longer needed, and unload them.
		*/
		void ClearUnique();
	}

}