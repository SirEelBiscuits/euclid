#include "AudioSystem.h"

namespace Audio {
	
	PositionVec2  Context::ListenerPosition{0_m, 0_m};
	btStorageType Context::angle{0.f};

	void Context::SetListenerPosition(PositionVec2 newPos, btStorageType newAngle) {
		ListenerPosition = newPos;
		angle = newAngle;
	}
}