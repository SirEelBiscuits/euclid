#include "Config.h"

namespace System {

	Config::Config() {
	}

	Config::Config(char const* filename) {
		luaX_dofile(s, filename);
	}

	Config::~Config() {
	}

	bool Config::IsValueSet(char const * name) const
	{
		lua_getglobal(s, name);
		auto ret = !lua_isnil(s, -1);
		lua_pop(s, 1);
		return ret;
	}

}
