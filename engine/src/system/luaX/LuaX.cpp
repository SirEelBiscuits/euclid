#include "../LuaX.h"

///////////////////////////////////////////////////////////////////////////////
// LuaXClasses

std::map<std::type_index, std::unique_ptr<luaX_ref>> luaX_typeTable;

luaX_ref::luaX_ref(lua_State *s)
	: s(s)
	, ref(luaL_ref(s, LUA_REGISTRYINDEX))
{}

void luaX_ref::push() {
	lua_rawgeti(s, LUA_REGISTRYINDEX, ref);
}

///////////////////////////////////////////////////////////////////////////////
// LuaXStack

template<>
int luaX_return<int>(lua_State *s) {
	auto ret = static_cast<int>(lua_tointeger(s, -1));
	lua_pop(s, 1);
	return ret;
}

template<>
float luaX_return<float>(lua_State *s) {
	auto ret = static_cast<float>(lua_tonumber(s, -1));
	lua_pop(s, 1);
	return ret;
}

template<>
double luaX_return<double>(lua_State *s) {
	auto ret = static_cast<double>(lua_tonumber(s, -1));
	lua_pop(s, 1);
	return ret;
}

template<>
bool luaX_return<bool>(lua_State *s) {
	auto ret = !!lua_toboolean(s, -1);
	lua_pop(s, 1);
	return ret;
}

template<>
std::string luaX_return<std::string>(lua_State *s) {
	auto ccp = lua_tostring(s, -1);
	if(nullptr == ccp)
		return "";
	auto ret = std::string(ccp);
	lua_pop(s, 1);
	return ret;
}

template<>
void luaX_push<int>(lua_State *s, int value) {
	lua_pushinteger(s, value);
}

template<>
void luaX_push<unsigned>(lua_State *s, unsigned value) {
	lua_pushinteger(s, value);
}

template<>
void luaX_push<float>(lua_State *s, float value) {
	lua_pushnumber(s, static_cast<lua_Number>(value));
}

template<>
void luaX_push<double>(lua_State *s, double value) {
	lua_pushnumber(s, static_cast<lua_Number>(value));
}

template<>
void luaX_push<bool>(lua_State *s, bool value) {
	lua_pushboolean(s, value);
}

template<>
void luaX_push<char const*>(lua_State *s, char const *value) {
	lua_pushstring(s, value);
}

template<>
void luaX_push<std::string>(lua_State *s, std::string value) {
	lua_pushstring(s, value.c_str());
}

template<>
void luaX_push<luaX_emptytable>(lua_State *s, luaX_emptytable em) {
	lua_createtable(s, em.narr, em.nrec);
}

template<>
void luaX_push<luaX_ref>(lua_State *s, luaX_ref ref) {
	ref.push();
}

///////////////////////////////////////////////////////////////////////////////
// luaX

void luaX_showErrors(lua_State* s, char const *name, int errCode) {
	switch(errCode) {
	case LUA_OK:
		return;
	case LUA_ERRSYNTAX:
		printf("Syntax error in %s:\n", name);
		break;
	case LUA_ERRRUN:
		printf("Runtime error in %s:\n", name);
		break;
	case LUA_ERRMEM:
		printf("Memory error in %s: \n", name);
		break;
	case LUA_ERRGCMM:
		printf("Garbage collection error in %s (this is likely unrelated to the file):\n", name);
		break;
	case LUA_ERRFILE:
		printf("Error opening file %s:\n", name);
		break;
	default:
		printf("Unknown error (type: %d) in %s:\n", errCode, name);
		break;
	}
	printf("%s\n", lua_tostring(s, -1));
	lua_pop(s, 1);
}

static int traceback(lua_State *s) {
	luaL_traceback(s, s, nullptr, 0);
	printf("%s\n", lua_tostring(s, -1));
	return 1;
}

int luaX_pcall(lua_State *s, int numArgs, int numReturns) {
#ifdef EUCLID_DEBUG
	auto x = lua_gettop(s);
	lua_pushcfunction(s, &traceback);
	auto ptr = 0 - numArgs - 2;
	lua_insert(s, ptr);
	auto ret = lua_pcall(s, numArgs, numReturns, ptr);
	if(ret == LUA_OK)
		lua_remove(s, 0 - numReturns - 1);
	else
		lua_remove(s, -2);
	ASSERT(lua_gettop(s) == x - numArgs  - 1 + (ret == LUA_OK ? numReturns : 1));
	return ret;
#else
	return lua_pcall(s, numArgs, numReturns, 0);
#endif
}

bool luaX_dofile(lua_State *s, char const *filename, int numReturns /* = 0 */) {
	auto err = luaL_loadfile(s, filename);
	if(err == LUA_OK) {
		err = luaX_pcall(s, 0, numReturns);
	}
	luaX_showErrors(s, filename, err);
	return err == LUA_OK;
}

int luaX_getglobal(lua_State *lua, char const *first) {
	lua_getglobal(lua, first);
	return 1;
}

int luaX_getlocal(lua_State *lua, char const *first) {
	lua_getfield(lua, -1, first);
	return 1;
}
