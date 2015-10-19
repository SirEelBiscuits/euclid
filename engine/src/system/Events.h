#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <vector>
#include <functional>
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

	namespace Events {
		enum class Types {
			Unknown,
			Quit,
		};
		void SetHandlers(std::function<void(Types, void*, void*)> callback);
	}
}