#include "Luaclid.h"

#include "system/LuaX.h"

#include "world/Map.h"
#include "system/Files.h"

#include "rendering/RenderingSystem.h"
#include "rendering/world/MapRenderer.h"
#include "rendering/world/TopDownMapRenderer.h"

template<>
Rendering::Color luaX_return<Rendering::Color>(lua_State *lua) {
	Rendering::Color ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.r = static_cast<int>(luaX_returnlocal<int>(lua, "r"));
	ret.g = static_cast<int>(luaX_returnlocal<int>(lua, "g"));
	ret.b = static_cast<int>(luaX_returnlocal<int>(lua, "b"));
	luaX_getlocal(lua, "a");
	if(lua_type(lua, -1) == LUA_TNIL) {
		ret.a = 255;
		lua_pop(lua, 1);
	} else
		ret.a = static_cast<int>(luaX_return<int>(lua));
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<Rendering::Color>(lua_State *lua, Rendering::Color c) {
	luaX_push(lua, luaX_emptytable{0, 4});
	luaX_settable(lua,
		"r", (int)c.r,
		"g", (int)c.g,
		"b", (int)c.b,
		"a", (int)c.a
	);
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
	luaX_settable(lua,
		"x", v.x,
		"y", v.y
	);
}

template<>
UVVec2 luaX_return<UVVec2>(lua_State *lua) {
	UVVec2 ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.x = luaX_returnlocal<Fix16>(lua, "x");
	ret.y = luaX_returnlocal<Fix16>(lua, "y");
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<UVVec2>(lua_State *lua, UVVec2 v) {
	luaX_push(lua, luaX_emptytable{0, 2});
	luaX_settable(lua,
		"x", v.x,
		"y", v.y
	);
}

template<>
PositionVec2 luaX_return<PositionVec2>(lua_State *lua) {
	PositionVec2 ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.x.val = luaX_returnlocal<btStorageType>(lua, "x");
	ret.y.val = luaX_returnlocal<btStorageType>(lua, "y");
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<PositionVec2>(lua_State *lua, PositionVec2 v) {
	luaX_push(lua, luaX_emptytable{0, 2});
	luaX_settable(lua,
		"x", v.x.val,
		"y", v.y.val
	);
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
	luaX_settable(lua,
		"x", v.x.val,
		"y", v.y.val,
		"z", v.z.val
	);
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
	luaX_settable(lua,
		"x", r.pos.x,
		"y", r.pos.y,
		"w", r.size.x,
		"h", r.size.y
	);
}

template<>
Rendering::World::View luaX_return<Rendering::World::View>(lua_State *lua) {
	Rendering::World::View ret;
	ASSERT(lua_type(lua, -1) == LUA_TTABLE);
	ret.eye = luaX_returnlocal<PositionVec3>(lua, "eye");
	ret.forward = RotationMatrix(luaX_returnlocal<btStorageType>(lua, "angle"));
	ret.sector =  luaX_returnlocal<World::Sector*>(lua, "sector");
	lua_pop(lua, 1);
	return ret;
}

template<>
void luaX_push<Rendering::World::View>(lua_State *lua, Rendering::World::View v) {
	luaX_push(lua, luaX_emptytable(0, 3));
	btStorageType angle = std::acos(v.forward.data[0][0]); //whatever
	luaX_settable(lua,
		"eye", v.eye,
		"sector", const_cast<World::Sector*>(v.sector),
		"angle", angle
	);
}

template<>
Mesi::Meters luaX_return<Mesi::Meters>(lua_State *lua) {
	return Mesi::Meters(luaX_return<btStorageType>(lua));
}

template<>
void luaX_push<Mesi::Meters>(lua_State *lua, Mesi::Meters value) {
	luaX_push(lua, value.val);
}

namespace System {
	namespace Luaclid {

		struct ExtraSpace {
			ExtraSpace(Rendering::Context &ctx)
				: mapRenderer(std::make_unique<Rendering::World::MapRenderer>(ctx))
			{}
			std::unique_ptr<Rendering::World::MapRenderer> mapRenderer{nullptr};
		};

		ExtraSpace* getExtraSpace(lua_State *s) {
			return *static_cast<ExtraSpace**>(lua_getextraspace(s));
		}

		using reloaderType = std::function<void(char const *)>;

		void RegisterTypes(lua_State *lua);
		void RegisterFunctions(lua_State *lua, Rendering::Context *ctx, reloaderType reloader);

		void SetUp(lua_State *lua, Rendering::Context &ctx, System::Config const &cfg) {
			auto reloader = [lua](char const* filename) {
				auto ret = luaX_dofile(lua, filename);
				ASSERT(ret);
			};
			for(auto s : {"luaclid.lua"}) {
				auto ret = luaX_dofile(lua, s);
				CRITICAL_ASSERT(ret);
				System::Events::RegisterFileToWatch(s, reloader);
			}

			//todo type safety, potential memory leak
			auto es = lua_getextraspace(lua);
			*(ExtraSpace**)es = new ExtraSpace(ctx);

			RegisterTypes(lua);
			RegisterFunctions(lua, &ctx, reloader);

			if(luaX_dofile(lua, "luaclid-late.lua"))
				System::Events::RegisterFileToWatch("luaclid-late.lua", reloader);

			{
				auto Gamelua = "Game.lua";
				auto ret = luaX_dofile(lua, Gamelua);
				CRITICAL_ASSERT(ret);
				System::Events::RegisterFileToWatch(Gamelua, reloader);
			}

			{
				auto startscript = cfg.GetValue<std::string>("startscript");
				auto ret = luaX_dofile(lua, startscript.c_str());
				CRITICAL_ASSERT(ret);
				System::Events::RegisterFileToWatch(startscript.c_str(), reloader);
			}
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
				ret.uvStart.x = luaX_returnlocal<Fix16>(lua, "uStart");
				ret.uvStart.y = luaX_returnlocal<Fix16>(lua, "vStart");
			}
			lua_pop(lua, 1);
			return ret;
		}

		std::unique_ptr<World::Map> LoadMap(lua_State *lua) {
			auto map = std::make_unique<World::Map>();
			if(lua_type(lua, -1) != LUA_TTABLE) {
				return map;
			}
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
					sec->lightLevel = luaX_returnlocal<btStorageType>(lua, "lightLevel");
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

		void GameInitialise(lua_State *lua, System::Config &cfg) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "Initialise");
			lua_gettop(lua);
			if(2 == pushed && lua_isfunction(lua, -1)) {
				luaX_push(lua, cfg.IsValueSet("startstate") ? cfg.GetValue<std::string>("startstate") : "" );

				// If 'extradata' was provided, this needs to be copied into a table for the last
				// parameter. Copying between two lua instances is a bastard though!
				cfg.PushExtraDataToLuaStack(lua);

				if(LUA_OK == luaX_pcall(lua, 2, 0) )
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

		void GameRender(lua_State *lua) {
			auto x = lua_gettop(lua);
			auto pushed = luaX_getglobal(lua, "Game", "Render");
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
			auto luaPosVec = luaX_registerClass<PositionVec2>(lua, "PositionVec2"
				, "x", &PositionVec2::x
				, "y", &PositionVec2::y
			);
			auto luaTexture = luaX_registerClass<Rendering::Texture>(lua, "Texture"
				, "width", &Rendering::Texture::w
				, "height", &Rendering::Texture::h
			);
			auto luaTextureInfo = luaX_registerClass<Rendering::TextureInfo>(lua, "TextureInfo"
				, "tex", &Rendering::TextureInfo::tex
				, "uvStart", &Rendering::TextureInfo::uvStart
			);

			////////////////////////////////////
			// Wall
			auto luaWall = luaX_registerClass<World::Wall>(lua, "Wall"
				, "portal", &World::Wall::portal
				, "start" , &World::Wall::start);
			luaWall.push();
			luaX_registerClassGetter(lua
				, "length", &World::Wall::length); // not const, so getter only must be forced
			luaX_registerClassMemberSpecial(lua
				, "topTex", &World::Wall::topTex);
			luaX_registerClassMemberSpecial(lua
				, "mainTex", &World::Wall::mainTex);
			luaX_registerClassMemberSpecial(lua
				, "bottomTex", &World::Wall::bottomTex);
			lua_pop(lua, 1);

			////////////////////////////////////
			// Sector
			auto luaSector = luaX_registerClass<World::Sector>(lua, "Sector"
					, "ceilHeight",        &World::Sector::ceilHeight
					, "floorHeight",       &World::Sector::floorHeight
					, "lightLevel",        &World::Sector::lightLevel
				// methods below here
					, "GetNumWalls",       &World::Sector::GetNumWalls
					, "InsertWallAfter",   &World::Sector::InsertWallAfter
					, "InsertWallBefore",  &World::Sector::InsertWallBefore
					, "DeleteWall",        &World::Sector::DeleteWall
					, "UpdateCentroid",    &World::Sector::UpdateCentroid
					, "UpdateLineLengths", &World::Sector::UpdateLineLengths
					, "FixWinding",        &World::Sector::FixWinding
					, "ReverseWinding",    &World::Sector::ReverseWinding
				);
			luaSector.push();
			luaX_registerClassGetter(lua
				, "centroid", &World::Sector::centroid); // not const, so getter only must be forced
			luaX_registerClassMemberSpecial(lua
				, "floor", &World::Sector::floor);
			luaX_registerClassMemberSpecial(lua
				, "ceiling", &World::Sector::ceiling);

			// methods below here

			luaX_registerClassMethodNonVoid<World::Sector, World::Wall*, 1, World::IDType>(lua
				, "GetWall", &World::Sector::GetWall);
			lua_pop(lua, 1);

			////////////////////////////////////
			// Map
			auto luaMap = luaX_registerClass<World::Map>(lua, "Map"
				, "GetNumSectors",       &World::Map::GetNumSectors
				, "GetSectorID",         &World::Map::GetSectorID
				, "AddNewSector",        &World::Map::AddNewSector
				, "RemoveSector",        &World::Map::RemoveSector
				, "GetNumVerts",         &World::Map::GetNumVerts
				, "GetVertID",           &World::Map::GetVertID
				, "AddNewVert",          &World::Map::AddNewVert
				, "RemoveVert",          &World::Map::RemoveVert
				, "RegisterAllTextures", &World::Map::RegisterAllTextures
			);
			luaMap.push();
			luaX_registerClassMethodNonVoid<World::Map, World::Sector*, 1, World::IDType>(lua
				, "GetSector", &World::Map::GetSector
			);
			luaX_registerClassMethodNonVoid<World::Map, World::Vert*, 1, World::IDType>(lua
				, "GetVert", &World::Map::GetVert
			);
			lua_pushcfunction(lua,
				[](lua_State *s) {
					//args(self, sectorID, position)
					lua_getfield(s, 1, "_data");
					auto map = luaX_touserdata<World::Map>(s, -1);
					lua_pop(s, 1);
					auto secID = static_cast<World::IDType>(lua_tointeger(s, 2) - 1);
					luaX_push(s,
						map->GetSector(secID)->barrow.CreateSprite(
							luaX_return<PositionVec3>(s)
						)
					);
					return 1;
				}
			);
			lua_setfield(lua, -2, "CreateSprite");

			lua_pushcfunction(lua,
				[](lua_State *s) {
					//args: (self, sprite)
					lua_getfield(s, 1, "_data");
					auto map = luaX_touserdata<World::Map>(s, -1);
					lua_pop(s, 1);
					lua_getfield(s, 2, "_data");
					auto spr = luaX_touserdata<World::Sprite>(s, -1);
					spr->GetSector()->barrow.DeleteSprite(*spr);
					return 0;
				}
			);
			lua_setfield(lua, -2, "DeleteSprite");

			lua_pop(lua, 1);

			/////////////////////////////////
			// Sprite
			auto luaSprite = luaX_registerClass<World::Sprite>(lua, "Sprite"
				, "_Move",            &World::Sprite::Move
				, "height",           &World::Sprite::height
				, "position",         &World::Sprite::position
				, "angle",            &World::Sprite::angle
			);
			luaSprite.push();

			//Move needs wrapping to transform the array index :(
			lua_pushcfunction(lua,
				[](lua_State *s) {
					luaX_push(s, luaX_return<int>(s) - 1);
					lua_getfield(s, 1, "_Move");
					lua_insert(s, 1);
					lua_call(s, 3, 0);
					return 0;
				}
			);
			lua_setfield(lua, -2, "Move");

			//set_texture(self, texture)
			lua_pushcfunction(lua,
				[](lua_State *s) {
					auto tex = luaX_return<Rendering::Texture*>(s);
					lua_getfield(s, 1, "_data");
					auto spr = luaX_touserdata<World::Sprite>(s, -1);
					spr->SetTexture(tex);
					return 0;
				}
			);
			lua_setfield(lua, -2, "set_texture");

			//set_textureAngle(self, texture, angle)
			lua_pushcfunction(lua,
				[](lua_State *s) {
					auto angle = luaX_return<btStorageType>(s);
					auto tex = luaX_return<Rendering::Texture*>(s);
					lua_getfield(s, 1, "_data");
					auto spr = luaX_touserdata<World::Sprite>(s, -1);
					spr->SetTextureAngle(tex, angle);
					return 0;
				}
			);
			lua_setfield(lua, -2, "set_textureAngle");

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
						if(System::Events::IsFileBeingWatched(filename.c_str()))
							return;
						System::Events::RegisterFileToWatch(filename.c_str(), reloader);
						auto ret = luaX_dofile(lua, filename.c_str());
						ASSERT(ret);
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

				//Draw.RectAlpha
				luaX_push(lua,
					static_cast<std::function<void(Rendering::ScreenRect, Rendering::Color)>>(
						[ctx](Rendering::ScreenRect rec, Rendering::Color c) {
							ctx->DrawRect(rec, c, true);
						}
					)
				);
				lua_setfield(lua, -2, "RectAlpha");

				//Draw.RectTextured
				luaX_push(lua,
					static_cast<std::function<void(Rendering::ScreenRect, Rendering::Texture*)>>(
						[ctx](Rendering::ScreenRect dest, Rendering::Texture *tex) {
							if(tex != nullptr)
								ctx->DrawRect(
									dest,
									tex,
									Rendering::UVRect{
										UVVec2(0_fp, 0_fp),
										UVVec2(Fix16(tex->w), Fix16(tex->h))
									},
									1,
									0
								);
						}
					)
				);
				lua_setfield(lua, -2, "RectTextured");

				//Draw.RectTexturedAlpha
				luaX_push(lua,
					static_cast<std::function<void(Rendering::ScreenRect, Rendering::Texture*)>>(
						[ctx](Rendering::ScreenRect dest, Rendering::Texture *tex) {
							if(tex != nullptr)
								ctx->DrawRectAlpha(
									dest,
									tex,
									Rendering::UVRect{
										UVVec2(0_fp, 0_fp),
										UVVec2(Fix16(tex->w), Fix16(tex->h))
									},
									1,
									0
								);
						}
					)
				);
				lua_setfield(lua, -2, "RectTexturedAlpha");

				//Draw.TopDownMap
				luaX_push(lua,
					static_cast<std::function<void(::World::Map *, Rendering::World::View, Rendering::Color)>>(
						[ctx](::World::Map *map, Rendering::World::View view, Rendering::Color c) {
							Rendering::World::TopDownMapRenderer::Render(*ctx, *map, view, c);
						}
					)
				);
				lua_setfield(lua, -2, "TopDownMap");


				//Draw.Map
				luaX_push(lua,
					static_cast<std::function<void(Rendering::World::View)>>(
						[mapRenderer = getExtraSpace(lua)->mapRenderer.get()]
						(Rendering::World::View v) {
							mapRenderer->Render(v);
						}
					)
				);
				lua_setfield(lua, -2, "Map");

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

				//Draw.GetScale
				luaX_push(lua,
					static_cast<std::function<int()>>(
						[ctx](){
							return ctx->GetScale();
						}
					)
				);
				lua_setfield(lua, -2, "GetScale");

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

				//Draw.GetPixel
				luaX_push(lua,
					static_cast<std::function<Rendering::Color(ScreenVec2)>>(
						[ctx](ScreenVec2 pos) {
							return ctx->ScreenPixel(pos.x, pos.y);
						}
					)
				);
				lua_setfield(lua, -2, "GetPixel");

				//Draw.SetPixel
				luaX_push(lua,
					static_cast<std::function<void(ScreenVec2, Rendering::Color)>>(
						[ctx](ScreenVec2 pos, Rendering::Color c) {
							ctx->ScreenPixel(pos.x, pos.y) = c;
						}
					)
				);
				lua_setfield(lua, -2, "SetPixel");

				//Draw.SetRenderScale
				luaX_push(lua,
					static_cast<std::function<void(unsigned)>>(
						[ctx](unsigned scale) {
							ctx->SetRenderScale(scale);
						}
					)
				);
				lua_setfield(lua, -2, "SetRenderScale");

				//Draw.GetRenderScale
				luaX_push(lua,
					static_cast<std::function<int()>>(
						[ctx]() {
							return ctx->GetScale();
						}
					)
				);
				lua_setfield(lua, -2, "GetRenderScale");

				lua_pop(lua, 1);
			}

			{
				luaX_getglobal(lua, "Game");

				//Game.OpenMap
				lua_pushcfunction(lua,
					(
						[]
						(lua_State *s) {
							luaX_push(s, LoadMap(s));
							return 1;
						}
					)
				);
				lua_setfield(lua, -2, "OpenMap");

				//Game.StoreMap
				lua_pushcfunction(lua,
					[](lua_State *s) {
						auto m = luaX_return<World::Map*>(s);
						StoreMap(s, *m);
						return 1;
					}
				);
				lua_setfield(lua, -2, "StoreMap");

				//Game.ShowMouse
				luaX_setlocal(lua, "ShowMouse", System::Input::SetMouseShowing);

				//Game.IsMouseShowing
				luaX_setlocal(lua, "IsMouseShowing", System::Input::GetMouseShowing);

				lua_pop(lua, 1);
			}

			{
				luaX_getglobal(lua, "Input");

				//Input.SetTextEditingMode
				luaX_setlocal(lua, "SetTextEditingMode", System::Input::SetTextEditingMode);

				lua_pop(lua, 1);
			}

			ASSERT(lua_gettop(lua) == x);
		}
	}
}

