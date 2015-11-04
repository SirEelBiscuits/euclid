#include "Luaclid.h"

#include "system/LuaX.h"

#include "world/Map.h"
#include "system/Files.h"

#include "Rendering/RenderingSystem.h"

template<>
Rendering::Color luaX_return<Rendering::Color>(lua_State *lua) {
	Rendering::Color ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.r = static_cast<int>(luaX_returnlocal<int>(lua, "r"));
	ret.g = static_cast<int>(luaX_returnlocal<int>(lua, "g"));
	ret.b = static_cast<int>(luaX_returnlocal<int>(lua, "b"));
	ret.a = static_cast<int>(luaX_returnlocal<int>(lua, "a"));
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<Rendering::Color>(lua_State *lua, Rendering::Color c) {
	luaX_push(lua, luaX_emptytable{0, 4});
	luaX_setlocal(lua, "r", (int)c.r);
	luaX_setlocal(lua, "g", (int)c.g);
	luaX_setlocal(lua, "b", (int)c.b);
	luaX_setlocal(lua, "a", (int)c.a);
}

template<>
ScreenVec2 luaX_return<ScreenVec2>(lua_State *lua) {
	ScreenVec2 ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.x = luaX_returnlocal<int>(lua, "x");
	ret.y = luaX_returnlocal<int>(lua, "y");
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<ScreenVec2>(lua_State *lua, ScreenVec2 v) {
	luaX_push(lua, luaX_emptytable{0, 2});
	luaX_setlocal(lua, "x", v.x);
	luaX_setlocal(lua, "y", v.y);
}

template<>
PositionVec3 luaX_return<PositionVec3>(lua_State *lua) {
	PositionVec3 ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.x.val = luaX_returnlocal<btStorageType>(lua, "x");
	ret.y.val = luaX_returnlocal<btStorageType>(lua, "y");
	ret.z.val = luaX_returnlocal<btStorageType>(lua, "z");
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<PositionVec3>(lua_State *lua, PositionVec3 v) {
	luaX_push(lua, luaX_emptytable{0, 2});
	luaX_setlocal(lua, "x", v.x.val);
	luaX_setlocal(lua, "y", v.y.val);
	luaX_setlocal(lua, "z", v.z.val);
}

template<>
Rendering::ScreenRect luaX_return<Rendering::ScreenRect>(lua_State *lua) {
	Rendering::ScreenRect ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.pos.x = luaX_returnlocal<int>(lua, "x");
	ret.pos.y = luaX_returnlocal<int>(lua, "y");
	ret.size.x = luaX_returnlocal<int>(lua, "w");
	ret.size.y = luaX_returnlocal<int>(lua, "h");
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<Rendering::ScreenRect>(lua_State *lua, Rendering::ScreenRect r) {
	luaX_push(lua, luaX_emptytable{0, 2});
	luaX_setlocal(lua, "x", r.pos.x);
	luaX_setlocal(lua, "y", r.pos.y);
	luaX_setlocal(lua, "w", r.size.x);
	luaX_setlocal(lua, "h", r.size.y);
}

namespace System {
	namespace Luaclid {

		using reloaderType = std::function<void(char const *)>;

		void RegisterTypes(lua_State *lua);
		void RegisterFunctions(lua_State *lua, Rendering::Context *ctx, reloaderType reloader);

		void SetUp(lua_State *lua, Rendering::Context &ctx, System::Config const &cfg) {
			auto reloader = [lua](char const* filename) {
				auto ret = luaX_dofile(lua, filename);
				ASSERT(ret);
			};
			auto ret = luaX_dofile(lua, "luaclid.lua");
			CRITICAL_ASSERT(ret);
			System::Events::RegisterFileToWatch("luaclid.lua", reloader);

			RegisterTypes(lua);
			RegisterFunctions(lua, &ctx, reloader);

			auto startscript = cfg.GetValue<std::string>("startscript");
			ret = luaX_dofile(lua, startscript.c_str());
			CRITICAL_ASSERT(ret);
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

				struct patchup {
					World::IDType secID, wallID, portalToID;
				};
				std::vector<patchup> patchupList;

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
							auto vertID = luaX_returnlocal<World::IDType>(lua, "start") - 1;
							wall->start = vertID >= 0? map->GetVert(vertID) : nullptr;
							auto portalID = luaX_returnlocal<World::IDType>(lua, "portal") - 1;
							//wall->portal = portalID >= 0 ? map->GetSector(portalID) : nullptr;
							if(portalID != World::IDType(-1)) {
								patchupList.emplace_back(patchup{World::IDType(secIdx - 1), World::IDType(wallIdx - 1), portalID});
							}
							
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

				for(auto &p : patchupList) {
					map->GetSector(p.secID)->GetWall(p.wallID)->portal = map->GetSector(p.portalToID);
				}
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

		void GameSaveState(lua_State *lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "SaveState");
			if(2 == pushed && lua_isfunction(lua, -1)) {
				luaX_pcall(lua, 0, 0);
				--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void GameLoadState(lua_State *lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "LoadState");
			if(2 == pushed && lua_isfunction(lua, -1)) {
				luaX_pcall(lua, 0, 0);
				--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void GamePostRender(lua_State *lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "PostRender");
			if(2 == pushed && lua_isfunction(lua, -1)) {
				if(LUA_OK == luaX_pcall(lua, 0, 0))
					--pushed;
			}
			lua_pop(lua, pushed);
			ASSERT(lua_gettop(lua) == x);
		}

		void RegisterTypes(lua_State *lua) {
			auto x = lua_gettop(lua);
			// we need to pre-declare these classes, as there are some circular references within some of them
			auto luaMeters = luaX_registerClass<Mesi::Meters>(lua
				,"val", &Mesi::Meters::val);
			auto luaPosVec2 = luaX_registerClass<PositionVec2>(lua);
			auto luaSector = luaX_registerClass<World::Sector>(lua);
			auto luaWall = luaX_registerClass<World::Wall>(lua
				,"portal", &World::Wall::portal
				,"start" , &World::Wall::start);
			auto luaTexture = luaX_registerClass<Rendering::Texture>(lua
				, "width", &Rendering::Texture::w
				, "height", &Rendering::Texture::h);
			auto luaMap = luaX_registerClass<World::Map>(lua);

			luaPosVec2.push();
			luaX_registerClassMemberSpecial(lua,
				"x", &PositionVec2::x );
			luaX_registerClassMemberSpecial(lua,
				"y", &PositionVec2::y );
			lua_pop(lua, 1);

			luaWall.push();
			luaX_registerClassGetterSpecial(lua
				, "length", &World::Wall::length);
			lua_pop(lua, 1);

			luaSector.push();
			luaX_registerClassMemberSpecial(lua
				, "ceilHeight", &World::Sector::ceilHeight);
			luaX_registerClassMemberSpecial(lua
				, "floorHeight", &World::Sector::floorHeight);
			luaX_registerClassGetterSpecial(lua
				, "ceilHeight", &World::Sector::centroid);
			lua_pop(lua, 1);

			luaMap.push();
			luaX_registerClassMethodNonVoid<World::Map, World::Sector*, 1, World::IDType>(lua, "GetSector", &World::Map::GetSector);
			lua_pop(lua, 1);

			ASSERT(lua_gettop(lua) == x);
		}

		void RegisterFunctions(lua_State *lua, Rendering::Context *ctx, reloaderType reloader) {
			ASSERT(lua);
			ASSERT(ctx);

			auto x = lua_gettop(lua);

			lua_pop(lua, luaX_setglobal(lua,
				"Game", "LoadAndWatchFile",
				//TODO: this cast is ugly but necessary in VS2015. what do?
				static_cast<std::function<void(std::string)>>(
					[lua, reloader](std::string filename) {
						ASSERT(luaX_dofile(lua, filename.c_str()));
						System::Events::RegisterFileToWatch(filename.c_str(), reloader);
					}
				)
			));
			
			{
				luaX_getglobal(lua, "Draw");

				//Draw.Line
				luaX_push(lua, 
					static_cast<std::function<void(ScreenVec2, ScreenVec2, Rendering::Color)>>(
						[ctx](ScreenVec2 start, ScreenVec2 end, Rendering::Color c) {
							ctx->DrawLine(start, end, c);
						}
					)
				);
				lua_setfield(lua, -2, "Line");

				//Draw.Rect
				luaX_push(lua,
					static_cast<std::function<void(Rendering::ScreenRect, Rendering::Color)>>(
						[ctx](Rendering::ScreenRect rec, Rendering::Color c) {
							ctx->DrawRect(rec, c);
						}
					)
				);
				lua_setfield(lua, -2, "Rect");

				//Draw.RectTextured
				luaX_push(lua,
					static_cast<std::function<void(Rendering::ScreenRect, Rendering::Texture*)>>(
						[ctx](Rendering::ScreenRect dest, Rendering::Texture *tex) {
							ctx->DrawRect(dest, tex, Rendering::UVRect{{0, 0}, {(btStorageType)tex->w, (btStorageType)tex->h}}, 1);
						}
					)
				);
				lua_setfield(lua, -2, "RectTextured");

				//Draw.RectTexturedAlpha
				luaX_push(lua,
					static_cast<std::function<void(Rendering::ScreenRect, Rendering::Texture*)>>(
						[ctx](Rendering::ScreenRect dest, Rendering::Texture *tex) {
							ctx->DrawRectAlpha(dest, tex, Rendering::UVRect{{0, 0}, {(btStorageType)tex->w, (btStorageType)tex->h}}, 1);
						}
					)	
				);
				lua_setfield(lua, -2, "RectTexturedAlpha");

				//Draw.GetWidth
				luaX_push(lua,
					static_cast<std::function<int()>>(
						[ctx](){
							return (int)ctx->GetWidth();
						}
					)
				);
				lua_setfield(lua, -2, "GetWidth");

				//Draw.GetHeight
				luaX_push(lua,
					static_cast<std::function<int()>>(
						[ctx](){
							return (int)ctx->GetHeight();
						}
					)
				);
				lua_setfield(lua, -2, "GetHeight");

				//Draw.GetTexture
				luaX_push(lua,
					static_cast<std::function<Rendering::Texture*(std::string)>>(
						[](std::string filename){
							return Rendering::TextureStore::GetTexture(filename.c_str()).get();
						}
					)
				);
				lua_setfield(lua, -2, "GetTexture");

				//Draw.Text
				luaX_push(lua,
					static_cast<std::function<void(ScreenVec2, Rendering::Texture*, std::string)>>(
						[ctx](ScreenVec2 topLeft, Rendering::Texture *tex, std::string text) {
							ctx->DrawText(topLeft, tex, text.c_str());
						}
					)
				);
				lua_setfield(lua, -2, "Text");

				lua_pop(lua, 1);
			}

			ASSERT(lua_gettop(lua) == x);
		}
	}
}

