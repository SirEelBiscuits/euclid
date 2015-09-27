#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <map>
#include <typeindex>
#include <memory>
POST_STD_LIB

#include <lua.hpp>

#define LUAX_NEED_LIBS true

/* custom smart pointer for incredibly stupid reasons */
/* lua state smart pointer. auto casts to lua_State* */
struct luaX_State {
	luaX_State() : s(luaL_newstate()) {}
	luaX_State(bool needLibs) : s(luaL_newstate()) { if(needLibs) luaL_openlibs(s); }
	~luaX_State() { lua_close(s); }

	lua_State *s;
	operator lua_State *() const {return s;}
};

/* helper type to push an empty table, or set one through other functions*/
struct luaX_emptytable {
	int narr;
	int nrec;
};

luaX_State luaX_newstate();

//TODO - ref counting, and destruction?
struct luaX_ref {
	luaX_ref(lua_State *s);
	int ref{LUA_NOREF};
	void push();
	lua_State *s;
};

extern std::map<std::type_index, std::unique_ptr<luaX_ref>> luaX_typeTable;
