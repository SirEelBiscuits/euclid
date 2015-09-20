#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <vector>
POST_STD_LIB

namespace System {
	namespace Input {
	
		enum class EventType {
			None,
			KeyDown,
			KeyUp,
			MouseDown,
			MouseUp
		};

		struct Event {
			union {
				int key{0};
				int button;
			};

			bool repeat{false};
			EventType type{EventType::None};
		};

		//implemented in platform code
		std::vector<Event> GetEvents();
	}
}