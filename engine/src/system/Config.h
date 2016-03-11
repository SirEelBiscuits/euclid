#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "LuaX.h"

namespace System {

class Config {
public:
	Config();
	Config(char const* filename);
	~Config();

	template<typename T>
	T GetValue(char const* name) const {
		return luaX_returnglobal<T>(s, name);
	};
	
	template<typename T>
	void SetValue(char const* name, T value) {
		luaX_setglobal(s, name, value);
	};

	void PushExtraDataToLuaStack(lua_State *guest) {
		luaX_push(guest, luaX_emptytable{});
		auto pushed = luaX_getglobal(s, "extradata");
		if(lua_type(s, -1) != LUA_TNIL) {
			lua_pushnil(s);
			while(lua_next(s, -2) != 0) {
				lua_insert(s, -2);
				auto key = luaX_return<std::string>(s);
				switch(lua_type(s, -1)) {
				case LUA_TNIL:
					lua_pushnil(guest);
					break;
				case LUA_TNUMBER: {
					auto val = luaX_return<double>(s);
					luaX_setlocal(guest, key.c_str(), val);
					break;
				}
				case LUA_TSTRING: {
					auto val = luaX_return<std::string>(s);
					luaX_setlocal(guest, key.c_str(), val);
					break;
				}
				case LUA_TBOOLEAN: {
					auto val = luaX_return<bool>(s);
					luaX_setlocal(guest, key.c_str(), val);
					break;
				}
				case LUA_TTABLE:
				case LUA_TFUNCTION:
				case LUA_TUSERDATA:
				case LUA_TLIGHTUSERDATA:
				default:
					// Other types not currently supported
					ASSERT(false);
				}
				luaX_push(s, key);
			}
		}
		lua_pop(s, pushed);
	}

	bool IsValueSet(char const *name) const;

private:
	luaX_State s;
};

}