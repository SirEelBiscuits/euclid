#include "SDLAudioSystem.h"

PRE_STD_LIB
#include <memory>
POST_STD_LIB

#include "SDL.h"
#include "SDL_mixer.h"

namespace Audio {

	std::map<int, PositionVec2> SDLContext::channelPositions{};

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

	void SDLContext::Update() {
		auto numChannels = Mix_AllocateChannels(-1); //best API ever
		for(int i = 0; i < numChannels; ++i) {
			if(!Mix_Playing(i))
				continue;
			auto relPos = SDLContext::channelPositions[i] - ListenerLocation;
			auto dist = relPos.Length();
			// 360 - because the winding is wrong in audio land, and it bugs out when given
			// angles below zero, but is fine with ones above 360!
			auto angle = 360 - std::atan2(- relPos.x.val, relPos.y.val) * 180 / M_PI;
			printf("%f\n", angle);
			Mix_SetPosition(i, angle, dist.val);
		}
	}
}