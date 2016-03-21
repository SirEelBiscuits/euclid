#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
POST_STD_LIB

#include "lib/BasicTypes.h"

namespace Audio {
	struct Context;

	std::unique_ptr<Context> GetContext();
	
	struct Context {
		Context() {}
		virtual ~Context() {}

		virtual void Update() = 0;
		virtual void SetListenerLocation(PositionVec2 newPos) { ListenerLocation = newPos; }

	protected:
		PositionVec2 ListenerLocation;
	};
}