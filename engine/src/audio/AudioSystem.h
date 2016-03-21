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
		virtual void SetListenerPosition(PositionVec2 newPos, btStorageType newAngle);

	protected:
		static PositionVec2 ListenerPosition;
		static btStorageType angle;
	};
}