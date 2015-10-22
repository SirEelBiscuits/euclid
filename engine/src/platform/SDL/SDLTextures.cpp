#include "rendering/Textures.h"

#include "SDL_image.h"

namespace Rendering {
	namespace TextureStore {
		TextureRef LoadTexture(char const *filename) {
			auto surface = IMG_Load(filename);
			ASSERT(surface);
			if(!surface)
				return TextureRef(nullptr);
			auto tex = std::make_shared<Texture>(surface->w, surface->h);
			auto bpp = surface->format->BytesPerPixel;
			CRITICAL_ASSERT(bpp >= 3 && bpp <= 4);

			for(decltype(surface->w) x = 0; x < surface->w; ++x)
				for(decltype(surface->h) y = 0; y < surface->h; ++y) {
					Color c;
					auto i = *(uint32_t*)((uint8_t*)surface->pixels + (x + y * surface->w) * bpp);
					c.a = (i & surface->format->Amask) >> surface->format->Ashift;
					c.r = (i & surface->format->Rmask) >> surface->format->Rshift;
					c.g = (i & surface->format->Gmask) >> surface->format->Gshift;
					c.b = (i & surface->format->Bmask) >> surface->format->Bshift;

					tex->pixel(x, y) = c;
				}
			SDL_FreeSurface(surface);

			return tex;
		}
	}
}