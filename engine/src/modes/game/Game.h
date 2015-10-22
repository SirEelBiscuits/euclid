#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <chrono>
POST_STD_LIB

#include "system/RunnableMode.h"
#include "system/LuaX.h"
#include "system/Events.h"

namespace System { namespace Events { enum class Types; } }
namespace World { class Map; }

namespace Modes {
	class Game : public System::RunnableMode {
	public:
		Game(Rendering::Context &ctx, System::Config &cfg);
		virtual ~Game();

		virtual void Run() override;
		virtual bool Update() override;

	private:
		void HandleInput();
		bool GameLogic();
		void RenderLogic();

		void SetUpAdditionalLuaStuff();

		void MarkLoopStart();
		void MarkLoopEnd();
		void StopLoop();

		void HandleEvents(System::Events::Types type, void *p1, void *p2);

		//Data

		bool done{false};
		luaX_State lua{LUAX_NEED_LIBS};

		std::chrono::high_resolution_clock::time_point oldTimePoint;

		std::unique_ptr<World::Map> curMap{nullptr};

		enum class InputRecordState {
			Normal,
			Recording,
			PlayingBack
		};
		InputRecordState inputRecordState{InputRecordState::Normal};
		std::vector<std::vector<System::Input::Event>> inputqueue{};
		unsigned inputqueuePosition{0u};
	};
}
