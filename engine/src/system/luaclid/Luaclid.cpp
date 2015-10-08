#include "Luaclid.h"

#include "world/Map.h"
#include "platform/Files.h"

namespace System {
	namespace Luaclid {

		using reloaderType = std::function<void(char const *)>;

		void RegisterTypes(lua_State *lua);
		void RegisterFunctions(lua_State *lua, reloaderType reloader);

		void SetUp(lua_State *lua, System::Config const &cfg) {
			auto reloader = [lua](char const* filename) {
				ASSERT(luaX_dofile(lua, filename));
			};
			CRITICAL_ASSERT(luaX_dofile(lua, "luaclid.lua"));
			System::Events::RegisterFileToWatch("luaclid.lua", reloader);

			RegisterTypes(lua);
			RegisterFunctions(lua, reloader);

			auto startscript = cfg.GetValue<std::string>("startscript");
			CRITICAL_ASSERT(luaX_dofile(lua, startscript.c_str()));
			System::Events::RegisterFileToWatch(startscript.c_str(), reloader);
		}

		void RegisterTypes(lua_State *lua) {
			// we need to pre-declare these classes, as there are some circular references within some of them
			auto luaMeters = luaX_registerClass<MesiType<btStorageType, 1, 0, 0>>(lua
				,"val", &Mesi::Meters::val);
			auto luaPosVec2 = luaX_registerClass<PositionVec2>(lua);
			auto luaWall = luaX_registerClass<World::Wall>(lua 
				//"portal", &World::Wall::portal,
				//"start", &World::Wall::start
			);


			luaPosVec2.push();
			luaX_registerClassGetterSpecial(lua,
				"x", &PositionVec2::x );
			luaX_registerClassSetterSpecial(lua,
				"x", &PositionVec2::x );
			luaX_registerClassGetterSpecial(lua,
				"y", &PositionVec2::y );
			luaX_registerClassSetterSpecial(lua,
				"y", &PositionVec2::y );
			lua_pop(lua, 1);

			luaWall.push();
			luaX_registerClassGetterSpecial(lua,
				"length", &World::Wall::length
			);
			lua_pop(lua, 1);

		}

		void RegisterFunctions(lua_State *lua, reloaderType reloader) {

			luaX_setglobal(lua,
				"Game", "LoadAndWatchFile",
				//TODO: this cast is ugly but necessary in VS2015. what do?
				static_cast<std::function<void(std::string)>>(
					[lua, reloader](std::string filename) {
						ASSERT(luaX_dofile(lua, filename.c_str()));
						System::Events::RegisterFileToWatch(filename.c_str(), reloader);
					}
				)
			);
		}
	}
}
