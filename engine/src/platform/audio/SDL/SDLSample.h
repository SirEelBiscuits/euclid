#include "lib/PortabilityHelper.h"

PREAMBLE

#include "audio/Sample.h"

struct Mix_Chunk;

namespace Audio {
	struct SDLSample : public Sample {
		SDLSample(Mix_Chunk &chunk);
		virtual ~SDLSample();
		
		virtual void Play() override;
		virtual void PlayAtLocation(PositionVec2 pos) override;
		Mix_Chunk * chunk;
	};
}