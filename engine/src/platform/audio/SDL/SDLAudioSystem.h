#include "lib/PortabilityHelper.h"

PREAMBLE

#include "audio/AudioSystem.h"

struct Mix_Chunk;

namespace Audio {
	struct SDLContext : public Context {
		SDLContext();
		virtual ~SDLContext() override;
	};
}