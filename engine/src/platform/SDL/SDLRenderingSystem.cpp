#include "SDLRenderingSystem.h"

PRE_STD_LIB
#include <cstdlib>
POST_STD_LIB

#include <SDL.h>

namespace Rendering {
	std::unique_ptr<Context> GetContext(
		unsigned Width,
		unsigned Height,
		unsigned Scale,
		bool FullScreen,
		char const *Title
	) {
		return std::unique_ptr<Context>(new SDLContext(Width, Height, Scale, FullScreen, Title));
	}

	template<>
	void sdlDeleter<SDL_Window>::operator()(SDL_Window* p) {
		SDL_DestroyWindow(p);
	}

	template<>
	void sdlDeleter<SDL_Renderer>::operator()(SDL_Renderer* p) {
		SDL_DestroyRenderer(p);
	}
	
	static std::shared_ptr<SDLSystem> sysCanonical;

	SDLSystem::SDLSystem()
	{
		if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			printf("%s", SDL_GetError());
			exit(EXIT_FAILURE);
		}
	}

	SDLSystem::~SDLSystem() {
		SDL_Quit();
	}

	SDLContext::SDLContext(
		unsigned Width,
		unsigned Height,
		unsigned Scale,
		bool FullScreen,
		char const *Title
	)	: Context(Width/Scale, Height/Scale, false)
		, ScreenWidth(Width)
		, ScreenHeight(Height)
	{
		if(nullptr == sysCanonical) {
			sysCanonical = std::make_shared<SDLSystem>();
			sys = sysCanonical;
		}

		win = winType(SDL_CreateWindow(
			Title,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			ScreenWidth,
			ScreenHeight,
			static_cast<Uint32>(SDL_WINDOW_SHOWN | (FullScreen ? SDL_WINDOW_FULLSCREEN : 0))
		));
		if(nullptr == win) {
			printf("%s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		ren = renType(SDL_CreateRenderer(win.get(), -1, 0));
		if(nullptr == ren) {
			printf("%s\n", SDL_GetError());
			exit(EXIT_FAILURE);
		}

		SetResolution(GetWidth(), GetHeight());
	}

	SDLContext::~SDLContext() {
		screen = nullptr;
		SDL_UnlockTexture(t);
		SDL_DestroyTexture(t);
		ren.reset();
		win.reset();
		sys.reset();
		if(sysCanonical.unique())
			sysCanonical.reset();
	}

	void SDLContext::Clear(Color c) {
		for(auto x = 0u; x < GetWidth(); ++x)
			for(auto y = 0u; y < GetHeight(); ++y)
				ScreenPixel(x, y) = c;
	}

	void SDLContext::SetResolution(unsigned Width, unsigned Height)
	{
		Context::Width = Width;
		Context::Height = Height;
		if(nullptr != t) {
			SDL_UnlockTexture(t);
			SDL_DestroyTexture(t);
		}
		t = SDL_CreateTexture(ren.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);
		int pitch;
		SDL_LockTexture(t, nullptr, (void**)&screen, &pitch);
	}

	void SDLContext::FlipBuffers()
	{
		SDL_UnlockTexture(t);
		SDL_RenderCopy(ren.get(), t, nullptr, nullptr);
		int pitch;
		SDL_LockTexture(t, nullptr, (void**)&screen, &pitch);
		SDL_RenderPresent(ren.get());
	}
}