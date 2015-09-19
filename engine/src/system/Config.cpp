#include "Config.h"

namespace System {

	Config::Config()
		: s(luaL_newstate())
	{
	}

	Config::Config(char const* filename)
		: s(luaL_newstate())
	{
		luaX_dofile(s, filename);
	}

	Config::~Config() {
		lua_close(s);
	}

	bool Config::IsValueSet(char const * name) const
	{
		lua_getglobal(s, name);
		auto ret = !lua_isnil(s, -1);
		lua_pop(s, 1);
		return ret;
	}

}
