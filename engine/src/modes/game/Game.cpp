#include "Game.h"

#include "platform/Files.h"
#include "platform/Events.h"
#include "platform/RenderingSystem.h"

#include "system/Config.h"

#include "world/Map.h"

namespace Modes {

	Game::Game(Rendering::Context &ctx, System::Config &cfg)
		: RunnableMode(ctx, cfg)
		, oldTimePoint(std::chrono::high_resolution_clock::now())
	{
		CRITICAL_ASSERT(luaX_dofile(lua, "luaclid.lua"));
		auto startscript = cfg.GetValue<std::string>("startscript");
		CRITICAL_ASSERT(luaX_dofile(lua, startscript.c_str()));

		
		auto reloader = [this](char const* filename) {
			ASSERT(luaX_dofile(lua, filename));
		};

		System::Events::RegisterFileToWatch("luaclid.lua", reloader);
		System::Events::RegisterFileToWatch(startscript.c_str(), reloader);

		luaX_setglobal(lua,
			"Game", "LoadAndWatchFile",
			//TODO: this cast is ugly but necessary in VS2015. what do?
			static_cast<std::function<void(std::string)>>(
				[this, reloader](std::string filename) {
					ASSERT(luaX_dofile(lua, filename.c_str()));
					System::Events::RegisterFileToWatch(filename.c_str(), reloader);
				}
			)
		);
	}

	Game::~Game() {
	}

	void Game::Run() {
		auto pushed = luaX_getglobal(lua, "Game", "Initialise");
		if(2 == pushed && lua_isfunction(lua, -1)) {
			auto err = lua_pcall(lua, 0, 0, 0);
			luaX_showErrors(lua, "Game.Initialise()", err);
			--pushed;
		}
		lua_pop(lua, pushed);
		System::Events::SetHandlers(
			[this](System::Events::Types type, void *p1, void *p2) {
				HandleEvents(type, p1, p2);
			}
		);
	}

	bool Game::Update() {
		System::Events::TickFileWatchers();

		auto ret = GameLogic();
		HandleInput();
		RenderLogic();

		return ret;
	}

	void Game::HandleInput() {
		auto input = System::Input::GetEvents();
		lua_getglobal(lua, "Game");
		if(lua_istable(lua, -1)) {
			lua_createtable(lua, input.size(), 0);

			for(auto i = 0u; i < input.size(); ++i) {
				luaX_push(lua, luaX_emptytable{0, 3});
				luaX_settable(lua,
					"key",       input[i].key,
					"keyRepeat", input[i].repeat,
					"eventType", (int)input[i].type
				);

				lua_seti(lua, -2, i + 1);
			}
			lua_setfield(lua, -2, "Input");
		}
		lua_pop(lua, 1); //remove "Game"
	}

	bool Game::GameLogic() {
		auto ret = true;

		auto timePoint = std::chrono::high_resolution_clock::time_point {std::chrono::high_resolution_clock::now()};
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(timePoint - oldTimePoint).count() / 1000000.0;
		oldTimePoint = timePoint;

		lua_getglobal(lua, "Game");
		if(lua_istable(lua, -1)) {
			lua_getfield(lua, -1, "Update");
			if(lua_isfunction(lua, -1)) {
				luaX_push(lua, dt);
				auto err = lua_pcall(lua, 1, 1, 0);
				luaX_showErrors(lua, "Game.Update()", err);
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

	static Rendering::Color clearColour{100, 149, 237, 0};
	void Game::RenderLogic() {
		ctx.Clear(clearColour);
		renderList.Render();
		ctx.FlipBuffers();
	}

	void Game::HandleEvents(System::Events::Types type, void *p1, void *p2) {
		using System::Events::Types;
		switch(type) {
		case Types::Quit:
			lua_getglobal(lua, "Game");
			if(lua_istable(lua, -1)) {
				lua_getfield(lua, -1, "Quit");
				if(lua_isfunction(lua, -1)) {
					auto err = lua_pcall(lua, 0, 0, 0);
					luaX_showErrors(lua, "Game.Quit()", err);
				} else {
					lua_pop(lua, 1);
				}
			}
			lua_pop(lua, 1);
			break;
		default:
			break;
		}
	}
}
