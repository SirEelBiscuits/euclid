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
	{
		System::Luaclid::SetUp(lua, ctx, cfg);
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

		SetUpAdditionalLuaStuff();
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
		if(lua_istable(lua, -1)) {
			luaX_push(lua, luaX_emptytable{(int)input.size(), 0});

			auto offset = 1u;
			for(auto i = 0u; i < input.size(); ++i) {

				luaX_push(lua, luaX_emptytable{0, 3});
				luaX_settable(lua,
					"key",       input[i].key,
					"keyRepeat", input[i].repeat,
					"eventType", (int)input[i].type
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

		static Rendering::World::MapRenderer mr(ctx);
		mr.Render(view);
		System::Luaclid::GamePostRender(lua);
		ctx.FlipBuffers();
	}

	void Game::SetUpAdditionalLuaStuff() {
		auto top = lua_gettop(lua);
		lua_getglobal(lua, "Game");
		//OpenMap
		{
			auto *innerF = static_cast<std::function<void(std::unique_ptr<World::Map>)>*> (
					lua_newuserdata(lua, sizeof(std::function<void(std::unique_ptr<World::Map>)>))
			);
			new(innerF) std::function<void(std::unique_ptr<World::Map>)>;
			*innerF = [this](std::unique_ptr<World::Map> map) {
				this->curMap = std::move(map);
			};

			auto closure = [](lua_State *s) {
				auto innerF = *static_cast<std::function<void(std::unique_ptr<World::Map>)>*>(
					lua_touserdata(s, lua_upvalueindex(1))
				);
				auto filename = luaX_return<std::string>(s);
				innerF(System::Luaclid::LoadMap(s));
				return 0;
			};

			lua_pushcclosure(lua, closure, 1);
			lua_setfield(lua, -2, "OpenMap");
		}
		//StoreMap
		{
			auto *innerF = static_cast<std::function<void(lua_State*)>*> (
				lua_newuserdata(lua, sizeof(std::function<void(lua_State*)>))
			);
			new(innerF) std::function<void(lua_State*)>;
			*innerF = [this](lua_State *s) {
				System::Luaclid::StoreMap(s, *this->curMap);
			};

			auto closure = [](lua_State *s) {
				auto innerF = *static_cast<std::function<void(lua_State*)>*>(
					lua_touserdata(s, lua_upvalueindex(1))
				);

				innerF(s);
				return 1;
			};

			lua_pushcclosure(lua, closure, 1);
			lua_setfield(lua, -2, "StoreMap");
		}
		//GetMap
		{
			luaX_push(lua,
				static_cast<std::function<World::Map*()>>(
					[this]() {
						return this->curMap.get();
					}	
				)
			);
			lua_setfield(lua, -2, "GetMap");
		}
		//SetView
		{
			luaX_push(lua,
				static_cast<std::function<void(PositionVec3, btStorageType, World::Sector *)>>(
					[this](PositionVec3 eyePos, btStorageType angle, World::Sector *sec) {
						this->view.eye = eyePos;
						this->view.forward = RotationMatrix(angle * 3.141592653f / 180.f); //todo magic numbers
						this->view.sector = sec;
					}	
				)
			);
			lua_setfield(lua, -2, "SetView");
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
		default:
			break;
		}
	}
}
