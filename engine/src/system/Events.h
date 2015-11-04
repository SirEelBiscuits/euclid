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
			MouseUp,
			MouseMove,
		};

		struct Event {
			union {
				int key{0};
				int button;
			};

			bool repeat{false};
			EventType type{EventType::None};
			int mouseMovX, mouseMovY, mouseMovXRel, mouseMovYRel;
		};

		//Implemented in platform code
		void SetMouseShowing(bool show);

		//Implemented in platform code
		bool ReturnOnKeyInput();

		//implemented in platform code
		std::vector<Event> GetEvents();
	}

	namespace Events {
		enum class Types {
			Unknown,
			Quit,
			InputLoopStart,
			InputLoopMarkEnd,
			InputLoopStop,
			DebugRenderingStart,
		};
		void SetHandlers(std::function<void(Types, void*, void*)> callback);
	}
}