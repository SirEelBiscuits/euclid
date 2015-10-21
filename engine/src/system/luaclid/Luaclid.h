#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <memory>
POST_STD_LIB

#include "system/LuaX.h"
#include "system/Config.h"

namespace World {
	class Map;
}

namespace System {
	namespace Luaclid {
		
		void SetUp(lua_State *s, System::Config const& cfg);

		std::unique_ptr<World::Map> LoadMap(lua_State *s, char const *filename);

	}
}
