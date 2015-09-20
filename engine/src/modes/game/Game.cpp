#include "Game.h"

#include "system/Config.h"
#include "system/Input.h"

namespace Modes {

Game::Game(Rendering::Context &ctx, System::Config &cfg)
	: RunnableMode(ctx, cfg)
	, oldTimePoint(std::chrono::high_resolution_clock::now())
{
	luaX_dofile(lua, cfg.GetValue<std::string>("startscript").c_str());
}

Game::~Game() {
}

void Game::Run() {
	lua_getglobal(lua, "Game");
	if(lua_istable(lua, -1)) {
		lua_getfield(lua, -1, "Initialise");
		if(lua_isfunction(lua, -1)) {
			auto err = lua_pcall(lua, 0, 0, 0);
			luaX_showErrors(lua, "Game.Initialise()", err);
		} else {
			lua_pop(lua, 1);
		}
	}
	lua_pop(lua, 1); //remove Game from the stack
}

bool Game::Update() {
	auto ret = true;

	auto timePoint = std::chrono::high_resolution_clock::time_point {std::chrono::high_resolution_clock::now()};
	auto dt = std::chrono::duration_cast<std::chrono::microseconds>(timePoint - oldTimePoint).count() / 1000000.0;
	oldTimePoint = timePoint;
	
	auto input = System::Input::GetEvents();
	lua_getglobal(lua, "Game");
	if(lua_istable(lua, -1)) {
		lua_createtable(lua, input.size(), 0);

		for(auto i = 0u; i < input.size(); ++i) {
			lua_createtable(lua, 0, 3);

			luaX_push(lua, input[i].key);
			lua_setfield(lua, -2, "key");
			luaX_push(lua, input[i].repeat);
			lua_setfield(lua, -2, "keyRepeat");
			luaX_push(lua, (int)input[i].type);
			lua_setfield(lua, -2, "eventType");

			lua_seti(lua, -2, i + 1);
		}

		lua_setfield(lua, -2, "Input");
	}

	if(lua_istable(lua, -1)) {
		lua_getfield(lua, -1, "Update");
		if(lua_isfunction(lua, -1)) {
			luaX_push(lua, dt);
			auto err = lua_pcall(lua, 1, 1, 0);
			luaX_showErrors(lua, "Game.Initialise()", err);
			if(LUA_OK == err) {
				ret = !!lua_toboolean(lua, -1);
				lua_pop(lua, 1);
			}
		} else {
			lua_pop(lua, 1); //remove "Update"
		}
	}
	lua_pop(lua, 1); //remove "Game"

	return ret;
}

}
