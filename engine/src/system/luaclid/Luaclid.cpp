#include "Luaclid.h"

#include "world/Map.h"
#include "system/Files.h"

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

		std::unique_ptr<World::Map> LoadMap(lua_State *lua, char const *filename) {
			luaX_dofile(lua, filename, 1);
			return LoadMap(lua);
		}

		Rendering::TextureInfo LoadTexInfo(lua_State *lua, char const *fieldName) {
			Rendering::TextureInfo ret;
			luaX_getlocal(lua, fieldName);
			if(lua_type(lua, -1) == LUA_TTABLE) {
				auto texName = luaX_returnlocal<std::string>(lua, "tex");
				ret.tex = texName.size() > 0 ? Rendering::TextureStore::GetTexture(texName.c_str()).get() : nullptr;
				ret.uvStart.x = luaX_returnlocal<int>(lua, "uStart");
				ret.uvStart.y = luaX_returnlocal<int>(lua, "vStart");
			}
			lua_pop(lua, 1);
			return ret;
		}

		std::unique_ptr<World::Map> LoadMap(lua_State *lua) {
			auto map = std::make_unique<World::Map>();
			{
				luaX_getlocal(lua, "verts");
				auto vertIdx = 1;
				do {
					lua_geti(lua, -1, vertIdx);
					if(lua_isnil(lua, -1)) {
						lua_pop(lua, 1);
						break;
					}
					auto v = map->AddNewVert();
					v->x.val = luaX_returnlocal<btStorageType>(lua, "x");
					v->y.val = luaX_returnlocal<btStorageType>(lua, "y");
					++vertIdx;
					lua_pop(lua, 1);
				} while (true);
				lua_pop(lua, 1);
			}
			{
				luaX_getlocal(lua, "sectors");
				auto secIdx = 1;
				do {
					lua_geti(lua, -1, secIdx);
					if(lua_isnil(lua, -1)) {
						lua_pop(lua, 1);
						break;
					}
					auto sec = map->AddNewSector();
					sec->ceilHeight.val = luaX_returnlocal<btStorageType>(lua, "ceilHeight");
					sec->floorHeight.val = luaX_returnlocal<btStorageType>(lua, "floorHeight");
					sec->floor = LoadTexInfo(lua, "floorTex");
					sec->ceiling = LoadTexInfo(lua, "ceilTex");

					{
						luaX_getlocal(lua, "walls");
						auto wallIdx = 1;
						do {
							lua_geti(lua, -1, wallIdx);
							if(lua_isnil(lua, -1)) {
								lua_pop(lua, 1);
								break;
							}
							auto wall = sec->InsertWallBefore(wallIdx-1);
							//indices supplied by lua are off by one because arrays start at 1
							auto vertID = luaX_returnlocal<int>(lua, "start") - 1;
							wall->start = vertID >= 0? map->GetVert(vertID) : nullptr;
							auto portalID = luaX_returnlocal<int>(lua, "portal") - 1;
							wall->portal = portalID >= 0 ? map->GetSector(portalID) : nullptr;
							
							wall->mainTex   = LoadTexInfo(lua, "mainTex");
							wall->bottomTex = LoadTexInfo(lua, "bottomTex");
							wall->topTex    = LoadTexInfo(lua, "topTex");

							wallIdx++;
							lua_pop(lua, 1);
						} while(true);
						lua_pop(lua, 1);
					}

					sec->FixWinding();
					sec->UpdateCentroid();
					sec->UpdateLineLengths();

					secIdx++;
					lua_pop(lua, 1);
				} while (true);
				lua_pop(lua, 1);
			}

			map->RegisterAllTextures();

			return map;
		}

		void StoreTexInfo(lua_State *lua, char const * fieldname, Rendering::TextureInfo const & texInfo) {
			luaX_push(lua, luaX_emptytable{0, 3});
			luaX_setlocal(lua, "tex", Rendering::TextureStore::GetTextureFilename(texInfo.tex));
			luaX_setlocal(lua, "uStart", texInfo.uvStart.x);
			luaX_setlocal(lua, "vStart", texInfo.uvStart.y);
			lua_setfield(lua, -2, fieldname);
		}

		void StoreMap(lua_State *lua, World::Map const &map) {
			luaX_push(lua, luaX_emptytable{0, 2});
			
			luaX_setlocal(lua, "verts", luaX_emptytable{static_cast<int>(map.GetNumVerts()) , 0});
			luaX_getlocal(lua, "verts");
			for(auto i = 0u; i < map.GetNumVerts(); ++i) {
				auto v = map.GetVert(i);
				luaX_push(lua, luaX_emptytable{0, 2});
				luaX_setlocal(lua, "x", v->x.val);
				luaX_setlocal(lua, "y", v->y.val);
				lua_seti(lua, -2, i+1); //pops the vert entry
			}
			lua_pop(lua, 1);

			luaX_setlocal(lua, "sectors", luaX_emptytable{static_cast<int>(map.GetNumSectors()), 0});
			luaX_getlocal(lua, "sectors");
			for(auto i = 0u; i < map.GetNumSectors(); ++i) {
				auto s = map.GetSector(i);
				luaX_push(lua, luaX_emptytable{0, 5});
				luaX_setlocal(lua, "floorHeight", s->floorHeight.val);
				luaX_setlocal(lua, "ceilHeight", s->ceilHeight.val);
				StoreTexInfo(lua, "floorTex", s->floor);
				StoreTexInfo(lua, "ceilTex", s->ceiling);

				luaX_setlocal(lua, "walls", luaX_emptytable{static_cast<int>(s->GetNumWalls()), 0});
				luaX_getlocal(lua, "walls");
				for(auto i = 0u; i < s->GetNumWalls(); ++i) {
					auto w = s->GetWall(i);
					luaX_push(lua, luaX_emptytable{0, 5});
					luaX_setlocal(lua, "start", map.GetVertID(w->start) + 1);
					luaX_setlocal(lua, "portal", map.GetSectorID(w->portal) + 1);
					StoreTexInfo(lua, "mainTex", w->mainTex);
					StoreTexInfo(lua, "bottomTex", w->bottomTex);
					StoreTexInfo(lua, "topTex", w->topTex);
					lua_seti(lua, -2, i+1);
				}
				lua_pop(lua, 1);

				lua_seti(lua, -2, i+1);
			}
			lua_pop(lua, 1);
		}

		bool GameUpdate(lua_State *lua, double dt) {
			auto ret = true;
			auto x = lua_gettop(lua);

			auto pushed = luaX_getglobal(lua, "Game", "Update");
			lua_gettop(lua);
			if(pushed == 2 && lua_isfunction(lua, -1)) {
				luaX_push(lua, dt);
				lua_gettop(lua);
				if(LUA_OK == luaX_pcall(lua, 1, 1)) {
					lua_gettop(lua);
					ret = luaX_return<bool>(lua);
					lua_gettop(lua);
					--pushed;
				}
			}
			lua_pop(lua, pushed);

			ASSERT(lua_gettop(lua) == x);
			return ret;
		}

		void GameQuit(lua_State *lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "Quit");
			if(2 == pushed && lua_isfunction(lua, -1)) {
				luaX_pcall(lua, 0, 0);
				--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void GameInitialise(lua_State *lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "Initialise");
			lua_gettop(lua);
			if(2 == pushed && lua_isfunction(lua, -1)) {
				if(LUA_OK == luaX_pcall(lua, 0, 0) )
					--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void GameSaveState(lua_State * lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "SaveState");
			if(2 == pushed && lua_isfunction(lua, -1)) {
				luaX_pcall(lua, 0, 0);
				--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void GameLoadState(lua_State * lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "LoadState");
			if(2 == pushed && lua_isfunction(lua, -1)) {
				luaX_pcall(lua, 0, 0);
				--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void RegisterTypes(lua_State *lua) {
			// we need to pre-declare these classes, as there are some circular references within some of them
			auto luaMeters = luaX_registerClass<Mesi::Meters>(lua
				,"val", &Mesi::Meters::val);
			auto luaPosVec2 = luaX_registerClass<PositionVec2>(lua);
			auto luaSector = luaX_registerClass<World::Sector>(lua);
			auto luaWall = luaX_registerClass<World::Wall>(lua); //setting portal and start here cause crashes !?


			luaPosVec2.push();
			luaX_registerClassMemberSpecial(lua,
				"x", &PositionVec2::x );
			luaX_registerClassMemberSpecial(lua,
				"y", &PositionVec2::y );
			lua_pop(lua, 1);

			luaWall.push();
			luaX_registerClassGetterSpecial(lua
				, "length", &World::Wall::length);
			//portal and start set here to prevent weird crashes. I wish I understood why this one happens..
			luaX_registerClassMember(lua
				, "portal", &World::Wall::portal);
			luaX_registerClassMember(lua
				, "start", &World::Wall::start);
			lua_pop(lua, 1);

			luaSector.push();
			luaX_registerClassMemberSpecial(lua
				, "ceilHeight", &World::Sector::ceilHeight);
			luaX_registerClassMemberSpecial(lua
				, "floorHeight", &World::Sector::floorHeight);
			luaX_registerClassGetterSpecial(lua
				, "ceilHeight", &World::Sector::centroid);
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
