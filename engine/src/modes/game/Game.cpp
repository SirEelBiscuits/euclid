#include "Game.h"

#include "rendering/RenderingSystem.h"

#include "system/Config.h"
#include "system/Files.h"
#include "system/Events.h"

#include "system/luaclid/Luaclid.h"
#include "world/Map.h"

namespace Modes {

	Game::Game(Rendering::Context &ctx, System::Config &cfg)
		: RunnableMode(ctx, cfg)
		, oldTimePoint(std::chrono::high_resolution_clock::now())
		, mapRenderer(ctx)
	{
		System::Luaclid::SetUp(lua, ctx, cfg);
		SetUpAdditionalLuaStuff();
	}

	Game::~Game() {
	}

	void Game::Run() {
		System::Luaclid::GameInitialise(lua);
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

		switch(inputRecordState) {
		case InputRecordState::Recording:
			inputqueue.push_back(input);
			break;
		case InputRecordState::PlayingBack:
			input = inputqueue[inputqueuePosition];
			++inputqueuePosition;
			if(inputqueuePosition >= inputqueue.size()) {
				inputqueuePosition = 0;
				System::Luaclid::GameLoadState(lua);
			}
			break;
		case InputRecordState::Normal:
			break;
		default:
			ASSERT(false);
		}

		lua_getglobal(lua, "Game");

		for(auto &i : input) {
			controls.HandleInput(i);
		}
		controls.PushInputInfo(lua);
		lua_setfield(lua, -2, "Controls");

		if(lua_istable(lua, -1)) {
			luaX_push(lua, luaX_emptytable{(int)input.size(), 0});

			auto offset = 1u;
			for(auto i = 0u; i < input.size(); ++i) {

				luaX_push(lua, luaX_emptytable{0, 3});
				luaX_settable(lua,
					"key",       input[i].key,
					"keyRepeat", input[i].repeat,
					"eventType", (int)input[i].type,
					"mouseMovX", input[i].mouseMovX,
					"mouseMovY", input[i].mouseMovY,
					"mouseMovXRel", input[i].mouseMovXRel,
					"mouseMovYRel", input[i].mouseMovYRel
				);

				lua_seti(lua, -2, i + offset);
			}
			lua_setfield(lua, -2, "Input");
		}
		lua_pop(lua, 1); //remove "Game"
	}

	bool Game::GameLogic() {
		auto timePoint = std::chrono::high_resolution_clock::time_point {std::chrono::high_resolution_clock::now()};
		auto dt = std::chrono::duration_cast<std::chrono::microseconds>(timePoint - oldTimePoint).count() / 1000000.0;
		oldTimePoint = timePoint;

		return System::Luaclid::GameUpdate(lua, dt);
	}

	static Rendering::Color clearColour{100, 149, 237, 0};
	void Game::RenderLogic() {
		ctx.Clear(clearColour);
		System::Luaclid::GameRender(lua);
		ctx.FlipBuffers();
	}

	void Game::SetUpAdditionalLuaStuff() {
		auto top = lua_gettop(lua);
		lua_getglobal(lua, "Game");
		//LoadControls
		{
			auto closure = [](lua_State *s) {
				auto controls = static_cast<System::Controls::Config*>(lua_touserdata(s, lua_upvalueindex(1)));
				controls->ClearControls();
				controls->LoadControls(s);
				return 0;
			};
			lua_pushlightuserdata(lua, &controls);
			lua_pushcclosure(lua, closure, 1);
			lua_setfield(lua, -2, "LoadControls");
		}
		//AddControls
		{
			auto closure = [](lua_State *s) {
				auto controls = static_cast<System::Controls::Config*>(lua_touserdata(s, lua_upvalueindex(1)));
				controls->LoadControls(s);
				return 0;
			};
			lua_pushlightuserdata(lua, &controls);
			lua_pushcclosure(lua, closure, 1);
			lua_setfield(lua, -2, "AddControls");
		}
		lua_pop(lua, 1);

		ASSERT(lua_gettop(lua) == top);
	}

	void Game::MarkLoopStart() {
		System::Luaclid::GameSaveState(lua);
		inputRecordState = InputRecordState::Recording;
		inputqueue.clear();
	}

	void Game::MarkLoopEnd() {
		if(inputRecordState == InputRecordState::Recording && inputqueue.size() > 0) {
			System::Luaclid::GameLoadState(lua);
			inputRecordState = InputRecordState::PlayingBack;
			inputqueuePosition = 0u;
		}	else 
			inputRecordState = InputRecordState::Normal;
	}

	void Game::StopLoop() {
		inputRecordState = InputRecordState::Normal;
		inputqueue.clear();
	}

	void Game::HandleEvents(System::Events::Types type, void *p1, void *p2) {
		using System::Events::Types;
		switch(type) {
		case Types::Quit:
			System::Luaclid::GameQuit(lua);
			break;
		case Types::InputLoopMarkEnd:
			MarkLoopEnd();
			break;
		case Types::InputLoopStart:
			MarkLoopStart();
			break;
		case Types::InputLoopStop:
			StopLoop();
			break;
		case Types::DebugRenderingStart:
			ctx.StartDebugRendering();
			break;
		default:
			break;
		}
	}
}
