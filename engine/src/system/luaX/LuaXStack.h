#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <utility>
POST_STD_LIB

#include <lua.hpp>


///////////////////////////////////////////////////////////////////////////////
// Misc

template<typename T>
T* luaX_newuserdata(lua_State *lua) {
	return static_cast<T*>(
		lua_newuserdata(lua, sizeof(T))
	);
}

template<typename T>
T* luaX_touserdata(lua_State *lua, int index) {
	return static_cast<T*>(lua_touserdata(lua, index));
}

///////////////////////////////////////////////////////////////////////////////
// Push

/* typed push onto the stack */
template<typename T>
void luaX_push(lua_State *s, T value);

/* typed push onto the stack */
template<typename T>
void luaX_push(lua_State *s, T *value) {
	auto t = lua_gettop(s);
	auto flxr = luaX_typeTable.find(typeid(T));
	if(flxr != luaX_typeTable.end()) {
		luaX_push(s, *flxr->second);
		lua_getfield(s, -1, "fromData");
		lua_insert(s, -2); //needs a self argument
		lua_pushlightuserdata(s, (void*)value);
		lua_pcall(s, 2, 1, 0);
	} else {
		lua_pushlightuserdata(s, value);
	}
	ASSERT(lua_gettop(s) == t + 1);
}

template<typename T, typename S>
void luaX_push(lua_State *s, std::unique_ptr<T, S> value) {
	auto t = lua_gettop(s);

	using ptrT = std::unique_ptr<T, S>;

	auto p = luaX_newuserdata<ptrT>(s);

	new(p) ptrT;
	*p = std::move(value);
	luaX_push(s, luaX_emptytable());
	auto f =
		[]
		(lua_State *lua) {
		auto t = lua_gettop(lua);
			auto s = lua_typename(lua, -1);
			auto p = luaX_touserdata<ptrT>(lua, -1);
			p->reset();
			return 0;
		};
	lua_pushcfunction(s, f);

	lua_setfield(s, -2, "__gc");
	lua_setmetatable(s, -2);

	//The above pushes the unique_ptr, but that's not actually useful to lua.
	// We need to also push the regular pointer (in the vein of luaX_push(lua_State*, T*))
	// and if we give it a reference to the unique_ptr, when it and its copies die, the unique object dies too.
	// hopefully

	luaX_push(s, p->get());
	lua_insert(s, -2);
	lua_setfield(s, -2, "__unqptr");

	ASSERT(lua_gettop(s) == t + 1);
}

template<typename T, typename... Args>
void luaX_push(lua_State *s, T value, Args... args) {
	luaX_push(s, value);
	luaX_push(s, args...);
}

template<typename... Args>
void luaX_push(lua_State *s, std::tuple<Args...> value) {
	using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<std::tuple<Args...>>>::value>;
	luaX_push(s, value, Indices{});
}

template<typename Tuple, size_t... I>
void luaX_push(lua_State *s, Tuple tuple, std::index_sequence<I...>) {
	luaX_push(s, std::get<I>(tuple)...);
}

template<typename T, int arity>
class luaX_registerfunction {
public:
};

template<typename Ret, typename... Args>
void luaX_push(lua_State *s, std::function<Ret(Args...)> f) {
	luaX_registerfunction<std::function<Ret(Args...)>, TypeMagic::arity<Ret>::value>::Register(s, f);
}

template<typename Ret, typename... Args>
void luaX_pushfunction(lua_State *s, std::function<Ret(Args...)> f) {
	luaX_registerfunction<std::function<Ret(Args...)>, TypeMagic::arity<Ret>::value>::Register(s, f);
}

template<typename... Args>
std::tuple<Args...> luaX_returntuplefromstack(lua_State *lua);

template<typename... Args>
class luaX_registerfunction<std::function<void(Args...)>, 0> {
public:
	static void Register(lua_State *s, std::function<void(Args...)> f) {
		using F = std::function<void(Args...)>;
		auto wrapper = [](lua_State *l) {
			if(lua_gettop(l) != sizeof...(Args)) {
				return luaL_error(l,
					"Bad call to function, %d arguments provided, %d expecterd",
					lua_gettop(l),
					sizeof...(Args)
				);
			}
			auto args = luaX_returntuplefromstack<Args...>(l);
			auto innerF = *luaX_touserdata<F>(l, lua_upvalueindex(1));
			TypeMagic::apply(innerF, args);
			return 0;
		};
		auto *callInner = luaX_newuserdata<F>(s);
		new(callInner) F;
		*callInner = f;
		lua_pushcclosure(s, wrapper, 1);
	}
};

template<typename Ret, int arity, typename... Args>
class luaX_registerfunction<std::function<Ret(Args...)>, arity> {
public:
	static void Register(lua_State *s, std::function<Ret(Args...)> f) {
		using F = std::function<Ret(Args...)>;
		auto wrapper = [](lua_State *l) {
			if(lua_gettop(l) != sizeof...(Args)) {
				return luaL_error(l,
					"Bad call to function, %d arguments provided, %d expecterd",
					lua_gettop(l),
					sizeof...(Args)
				);
			}
			auto args = luaX_returntuplefromstack<Args...>(l);
			auto innerF = *luaX_touserdata<F>(l, lua_upvalueindex(1));
			luaX_push(l, TypeMagic::apply(innerF, args));
			return arity;
		};
		auto *callInner = luaX_newuserdata<F>(s);
		new(callInner) F;
		*callInner = f;
		lua_pushcclosure(s, wrapper, 1);
	}
};

template<>
class luaX_registerfunction<std::function<void()>, 0> {
public:
	static void Register(lua_State *s, std::function<void()> f) {
		using F = std::function<void()>;
		auto wrapper = [](lua_State *l) {
			if(lua_gettop(l) != 0) {
				return luaL_error(l,
					"Bad call to function, %d arguments provided, %d expecterd",
					lua_gettop(l),
					0
				);
			}
			auto innerF = *luaX_touserdata<F>(l, lua_upvalueindex(1));
			innerF();
			return 0;
		};
		auto *callInner = luaX_newuserdata<F>(s);
		new(callInner) F;
		*callInner = f;
		lua_pushcclosure(s, wrapper, 1);
	}
};

template<typename Ret, int arity>
class luaX_registerfunction<std::function<Ret()>, arity> {
public:
	static void Register(lua_State *s, std::function<Ret()> f) {
		using F = std::function<Ret()>;
		auto wrapper = [](lua_State *l) {
			if(lua_gettop(l) != 0) {
				return luaL_error(l,
					"Bad call to function, %d arguments provided, %d expecterd",
					lua_gettop(l),
					0
				);
			}
			auto innerF = *luaX_touserdata<F>(l, lua_upvalueindex(1));
			luaX_push(l, innerF());
			return arity;
		};
		auto *callInner = luaX_newuserdata<F>(s);
		new(callInner) F;
		*callInner = f;
		lua_pushcclosure(s, wrapper, 1);
	}
};

///////////////////////////////////////////////////////////////////////////////
// Return

/* returns the value from the top of the stack. the item is popped from the stack */
template<typename T>
std::enable_if_t<!std::is_pointer<T>::value, T> luaX_return(lua_State *s);

/* returns the value from the top of the stack. the item is popped from the stack */
template<typename T>
std::enable_if_t<std::is_pointer<T>::value, T> luaX_return(lua_State *s) {
	void *data{nullptr};
	if(lua_isuserdata(s, -1)) {
		data = lua_touserdata(s, -1);
	}
	else if(lua_istable(s, -1)) {
		luaX_getlocal(s, "_data");
		if(lua_isuserdata(s, -1))
			data = lua_touserdata(s, -1);
		lua_pop(s, 1);
	}
	lua_pop(s, 1);
	return static_cast<T>(data);
}

/* returns the requested value. Stack ends up in the same state as at the calling point */
template<typename T>
T luaX_returnglobal(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	return luaX_return<T>(s);
}

template<typename T>
T luaX_returnlocal(lua_State *s, char const *name) {
	luaX_getlocal(s, name); //no need to check return - if the field doesn't exist, it'll get nil
	return luaX_return<T>(s);
}

template<typename Tuple, int I>
class luaX_returntuplefromstackInner;

template<typename... Args>
std::tuple<Args...> luaX_returntuplefromstack(lua_State *lua) {
		std::tuple<Args...> ret;
		luaX_returntuplefromstackInner<std::tuple<Args...>, sizeof...(Args) - 1>::Fn(lua, ret);
		return ret;
}

template<typename Tuple, int I>
class luaX_returntuplefromstackInner {
public:
	static void Fn(lua_State *lua, Tuple& ret) {
		std::get<I>(ret) = luaX_return<typename std::tuple_element<I, Tuple>::type>(lua);
		luaX_returntuplefromstackInner<Tuple, I-1>::Fn(lua, ret);
	}
};

template<typename Tuple>
class luaX_returntuplefromstackInner<Tuple, 0> {
public:
	static void Fn(lua_State *lua, Tuple& ret) {
		std::get<0>(ret) = luaX_return<typename std::tuple_element<0, Tuple>::type>(lua);
	}
};


///////////////////////////////////////////////////////////////////////////////
// Get

/* returns the number of items the stack gained during this call*/
int luaX_getglobal(lua_State *lua, char const *first);

/* returns the number of items the stack gained during this call*/
template<typename... Types>
int luaX_getglobal(lua_State *lua, char const *first, Types... rest) {
	luaX_getglobal(lua, first);
	if(lua_istable(lua, -1))
		return 1 + luaX_getlocal(lua, rest...);
	return 1;
}

/* returns the number of items the stack gained during this call*/
template<typename... Types>
int luaX_getlocal(lua_State *lua, char const *first, Types... rest) {
	lua_getfield(lua, -1, first);
	if(lua_istable(lua, -1))
		return 1 + luaX_getlocal(lua, rest...);
	return 1;
}

/* returns the number of items the stack gained during this call*/
int luaX_getlocal(lua_State *lua, char const *first);

///////////////////////////////////////////////////////////////////////////////
// Set

/* returns the number of items the stack gained during this call*/
template<typename T>
int luaX_setglobal(lua_State *s, char const *name, T value) {
	luaX_push(s, value);
	lua_setglobal(s, name);
	return 0;
}

/* returns the number of items the stack gained during this call*/
template<typename... Args>
int luaX_setglobal(lua_State* s, char const *name, Args... args) {
	lua_getglobal(s, name);
	if(lua_istable(s, -1))
		return 1 + luaX_setlocal(s, args...);
	return 1;
}

/* returns the number of items the stack gained during this call*/
template<typename... Args>
int luaX_setlocal(lua_State *lua, char const *firstKey, Args... args) {
	lua_getfield(lua, -1, firstKey);
	if(lua_istable(lua, -1))
		return 1 + luaX_setlocal(lua, args...);
	return 1;
}

//the fact this is identical to luaX_settable(lua_State, char const*, T) is a coincidence
// and they should remain separate(!)
/* returns the number of items the stack gained during this call*/
template<typename T>
int luaX_setlocal(lua_State *lua, char const *key, T val) {
	luaX_push(lua, val);
	lua_setfield(lua, -2, key);
	return 0;
}

/* Designed to set multiple key-value pairs on a single table
	args like:
		"key", someVal, "key2", someOtherVal"
	and so forth.
	expects the top of the stack to contain the table to be modified
*/
template<typename T, typename... Args>
void luaX_settable(lua_State *lua, char const *firstKey, T val, Args... args) {
	luaX_settable(lua, firstKey, val);
	luaX_settable(lua, args...);
}

//identical to base case for luaX_setlocal by coincidence, but they must remain separate
/* base case for luaX_settable*/
template<typename T>
void luaX_settable(lua_State *lua, char const *key, T val) {
	luaX_push(lua, val);
	lua_setfield(lua, -2, key);
}