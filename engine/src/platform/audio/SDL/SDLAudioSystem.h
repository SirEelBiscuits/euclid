#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <map>
POST_STD_LIB

#include "audio/AudioSystem.h"

struct Mix_Chunk;

namespace Audio {
	struct SDLContext : public Context {
		SDLContext();
		virtual ~SDLContext() override;

		virtual void Update() override;

		static std::map<int, PositionVec2> channelPositions;
	};
}