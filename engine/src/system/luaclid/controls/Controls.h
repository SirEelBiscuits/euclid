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
			virtual void ClearState() = 0;
			virtual std::unique_ptr<IInput> Copy() = 0;
		};
		
		enum class Mask {
			None      = 0,
			ShiftUp   = 1 << 0,
			ShiftDown = 1 << 1,
			CtrlUp    = 1 << 2,
			CtrlDown  = 1 << 3,
		};

		class Button : public IInput {
		public:
			Button(std::string name, int key, bool isMouse, Mask mask = Mask::None);
			virtual void ClearState() override;
			virtual std::unique_ptr<IInput> Copy() override;
			virtual void HandleEvent(Input::Event event) override;
			virtual void SetValue(lua_State *lua) override;

		private:
			std::string name;
			int key;
			enum {Pressed, UnPressed} state;
			bool newlyPressed;
			bool newlyReleased;
			bool isMouse;
			bool requireShiftDown{false};
			bool requireShiftUp  {false};
			bool requireCtrlDown {false};
			bool requireCtrlUp   {false};
		};

		class Axis : public IInput {
		public:
			Axis(std::string name, int posKey, int negKey);
			virtual void ClearState() override;
			virtual std::unique_ptr<IInput> Copy() override;
			virtual void HandleEvent(Input::Event event) override;
			virtual void SetValue(lua_State *lua) override;

		private:
			std::string name;
			int posKey, negKey;
			int state;
		};

		enum class AxisDirection {x,y};
		class MouseAxis : public IInput {
		public:
			MouseAxis(std::string name, AxisDirection axis, float scale, float maxMagnitude);
			virtual void ClearState() override;
			virtual std::unique_ptr<IInput> Copy() override;
			virtual void HandleEvent(Input::Event event) override;
			virtual void SetValue(lua_State *lua) override;

		private:
			std::string name;
			AxisDirection axis;
			float scale;
			float maxMagnitude;
			float state;
		};

		class MouseAxisRel : public IInput {
		public:
			MouseAxisRel(std::string name, AxisDirection axis, float scale, float maxMagnitude);
			virtual void ClearState() override;
			virtual std::unique_ptr<IInput> Copy() override;
			virtual void HandleEvent(Input::Event event) override;
			virtual void SetValue(lua_State *lua) override;

		private:
			std::string name;
			AxisDirection axis;
			float scale;
			float maxMagnitude;
			float state;
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

			void ClearInputState();

			std::unique_ptr<Config> Copy();
		private:
			std::vector<std::unique_ptr<IInput>> inputList;
		};
	}
}
