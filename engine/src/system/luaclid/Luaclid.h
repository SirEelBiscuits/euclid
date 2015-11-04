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

namespace Rendering {
	struct Context;
}

namespace System {
	namespace Luaclid {
		
		void SetUp(lua_State *s, Rendering::Context &ctx, System::Config const& cfg);
		
		std::unique_ptr<World::Map> LoadMap(lua_State *lua, char const *filename);
		std::unique_ptr<World::Map> LoadMap(lua_State *lua);
		void StoreMap(lua_State *lua, World::Map const &map);

		//wrappers for Lua functions
		bool GameUpdate(lua_State *lua, double dt);
		void GameQuit(lua_State *lua);
		void GameInitialise(lua_State *lua);

		void GameSaveState(lua_State *lua);
		void GameLoadState(lua_State *lua);

		void GameRender(lua_State *lua);
	}
}
