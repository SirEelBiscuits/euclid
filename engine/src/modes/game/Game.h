#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <chrono>
POST_STD_LIB

#include "system/RunnableMode.h"
#include "system/LuaX.h"

namespace Modes {
	class Game : public System::RunnableMode {
	public:
		Game(Rendering::Context &ctx, System::Config &cfg);
		virtual ~Game();

		virtual void Run() override;
		virtual bool Update() override;

	private:
		bool done{false};
		luaX_State lua{LUAX_NEED_LIBS};

		std::chrono::high_resolution_clock::time_point oldTimePoint;
	};
}