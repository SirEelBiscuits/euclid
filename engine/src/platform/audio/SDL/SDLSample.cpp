#include "SDLSample.h"

#include "SDL_mixer.h"
#include "SDLAudioSystem.h"

namespace Audio {
	SDLSample::SDLSample(Mix_Chunk &chunk)
		: chunk(&chunk)
	{}
	SDLSample::~SDLSample() {
		Mix_FreeChunk(chunk);
	}

	void SDLSample::Play() {
		Mix_PlayChannel(-1, chunk, 0);
	}

	void SDLSample::PlayAtLocation(PositionVec2 pos) {
		auto channel = Mix_PlayChannel(-1, chunk, 0);
		SDLContext::channelPositions[channel] = pos;
		SDLContext::UpdateSoundChannel(channel);
	}

	namespace SampleStore {
		SampleRef LoadSample(char const *filename) {
			auto *chunk = Mix_LoadWAV(filename);
			if(nullptr != chunk)
				return SampleRef(new SDLSample(*chunk));
			return SampleRef{};
		}
	}
}