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

		std::unique_ptr<World::Map> LoadMap(lua_State *lua);

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
