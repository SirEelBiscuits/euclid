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
			TextInput,
		};

		struct Event {
			union {
				int key{0};
				int button;
			};
			std::string textInput;
			int mouseMovX, mouseMovY, mouseMovXRel, mouseMovYRel;
			EventType type{EventType::None};
			bool repeat{false};
		};

		//Implemented in platform code
		void SetMouseShowing(bool show);

		//Implemented in platform code
		bool ReturnOnKeyInput();
		
		//implemented in platform code
		std::vector<Event> GetEvents();
		
		//implemented in platform code
		bool IsShiftDown();
		
		//implemented in platform code
		bool IsCtrlDown();
		
		//implemented in platform code
		void SetTextEditingMode(bool SetOn);
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