#include "SDLAudioSystem.h"

PRE_STD_LIB
#include <memory>
POST_STD_LIB

#include "SDL.h"
#include "SDL_mixer.h"

namespace Audio {
	std::unique_ptr<Context> GetContext() {
		return std::make_unique<SDLContext>();
	}

	SDLContext::SDLContext() {
		SDL_Init(SDL_INIT_AUDIO);
		Mix_OpenAudio(
			MIX_DEFAULT_FREQUENCY,
			MIX_DEFAULT_FORMAT,
			MIX_DEFAULT_CHANNELS,
			4096
		);
	}

	SDLContext::~SDLContext() {
		Mix_CloseAudio();
	}
}