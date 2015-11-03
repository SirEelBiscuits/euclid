#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <vector>
#include <memory>
POST_STD_LIB

#include "system/Events.h"
#include "system/luaclid/Luaclid.h"

namespace System {
	namespace Controls {

		class IInput {
		public:
			virtual void HandleEvent(Input::Event event) = 0;
			virtual void SetValue(lua_State *lua) = 0;
		};

		class Button : public IInput {
		public:
			Button(std::string name, int key);
			virtual void HandleEvent(Input::Event event) override;
			virtual void SetValue(lua_State *lua) override;

		private:
			std::string name;
			int key;
			enum {Pressed, UnPressed} state;
			bool newlyPressed;
			bool newlyReleased;
		};

		class Axis : public IInput {
		public:
			Axis(std::string name, int posKey, int negKey);
			virtual void HandleEvent(Input::Event event) override;
			virtual void SetValue(lua_State *lua) override;

		private:
			std::string name;
			int posKey, negKey;
			int state;
		};

		class Config {
		public:
			void HandleInput(Input::Event e);
			/**
				Loads controls from a table assumed to be at the top of the stack at the calling point
			 */
			void LoadControls(lua_State *lua);
			void ClearControls() { inputList.clear(); }
			void PushInputInfo(lua_State *lua);
		private:
			std::vector<std::unique_ptr<IInput>> inputList;
		};
	}
}
