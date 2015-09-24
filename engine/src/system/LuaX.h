#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <string>
#include <memory>
#include <type_traits>
#include <functional>
POST_STD_LIB

#include <lua.hpp>

#include "lib/TypeMagic.h"

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

struct luaX_ref {
	luaX_ref(lua_State *s);
	int ref{LUA_NOREF};
	void push();
	lua_State *s;
};

/* returns the requested value. Stack ends up in the same state as at the calling point */
template<typename T>
T luaX_returnglobal(lua_State *s, char const *name) {
	lua_getglobal(s, name);
	return luaX_return<T>(s);
}

/* returns the value from the top of the stack. the item is popped from the stack */
template<typename T>
std::enable_if_t<!std::is_pointer<T>::value, T> luaX_return(lua_State *s);

/* returns the value from the top of the stack. the item is popped from the stack */
template<typename T>
std::enable_if_t<std::is_pointer<T>::value, T> luaX_return(lua_State *s) {
	static_assert(std::is_pointer<T>::value, "T must be a pointer");
	auto ud = lua_touserdata(s, -1);
	lua_pop(s, 1);
	return static_cast<T>(ud);
}

/* typed push onto the stack */
template<typename T>
void luaX_push(lua_State *s, T value);

/* typed push onto the stack */
template<typename T>
void luaX_push(lua_State *s, T *value) {
	lua_pushlightuserdata(s, value);
	/* ::TODO:: 
		use a map of type_info -> luaX_ref to get a function like Class:FromUserdata
		to turn it into something usable if possible? maybe?
	*/
}

/* if errCode is not LUA_ERROK, this will print the error at the top of the stack and pop it */
void luaX_showErrors(lua_State* s, char const *name, int errCode);

/* returns true on success. prints all error information available */
bool luaX_dofile(lua_State *s, char const *filename);

/* returns the number of items the stack gained during this call*/
template<typename... Types>
int luaX_getglobal(lua_State *lua, char const *first, Types... rest) {
	luaX_getglobal(lua, first);
	if(lua_istable(lua, -1))
		return 1 + luaX_getlocal(lua, rest...);
	return 1;
}

/* returns the number of items the stack gained during this call*/
int luaX_getglobal(lua_State *lua, char const *first);

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
		return 1 + luaX_setlocal(s, vargs...);
	return 1;
}

/* returns the number of items the stack gained during this call*/
template<typename... Args>
int luaX_setlocal(lua_State *lua, char const *firstKey, Args... args) {
	lua_getfield(lua, -1, firstKey);
	if(lua_istable(lua, -1))
		return 1 + luaX_setlocalobj(lua, args);
	return 1;
}

//the fact this is identical to luaX_settable(lua_State, char const*, T) is a coincidence
// and they should remain separate(!)
/* returns the number of items the stack gained during this call*/
template<typename T>
int luaX_setlocal(lua_State *lua, char const *key, T val) {
	luaX_push(val);
	lua_setfield(lua, -2, key);
	return 0;
}

template<typename... Args>
void luaX_createtable(lua_State *lua, int narr, int nrec, Args... args) {
	lua_createtable(lua, narr, nrec);
	luaX_settable(lua, args...);
}

template<typename... Args>
void luaX_createtable(lua_State *lua, Args... args) {
	lua_createtable(lua, 0, 0);
	luaX_settable(lua, args...);
}

template<typename T, typename... Args>
void luaX_settable(lua_State *lua, char const *firstKey, T val, Args... args) {
	luaX_settable(lua, firstKey, val);
	luaX_settable(lua, args...);
}

template<typename T>
void luaX_settable(lua_State *lua, char const *key, T val) {
	luaX_push(lua, val);
	lua_setfield(lua, -2, key);
}

template<typename T, typename... Args>
luaX_ref luaX_registerClass(lua_State *lua, Args... args) {
	luaX_getglobal(lua, "CreateNativeClass");
	lua_pcall(lua, 0, 1, 0);
	luaX_registerClassMethod(lua, args...);

	//Need to register the constructor here, or something...
	return luaX_ref(lua);
}

template<typename T, typename... Args>
void luaX_registerClassMethod(lua_State *lua, char const *name, T member, Args... args ) {
	luaX_registerClassMethod(lua, name, member);
	luaX_registerClassMethod(lua, args...);
}

template<
	typename C,
	typename T,
	typename = std::enable_if_t<!std::is_function<T>::value, T>
>
void luaX_registerClassMethod(lua_State *lua, char const *name, T C::* member) {
	luaX_registerClassGetter(lua, name, member);
	luaX_registerClassSetter(lua, name, member);
}

template<
	typename C,
	typename T,
	typename = std::enable_if_t<!std::is_function<T>::value, T>
>
void luaX_registerClassMethod(lua_State *lua, char const *name, T const C::* member) {
	luaX_registerClassGetter(lua, name, member);
}

template<typename C, typename T>
void luaX_registerClassGetter(lua_State *lua, char const *name, T const C::* member) {
	//expects an argument of lightuserdata, type C*
	auto getter = [](lua_State *l) {
		luaX_getlocal(l, "_data");
		auto member = (*static_cast<std::function<T(C*)>*>(
			lua_touserdata(l, lua_upvalueindex(1))
		))(static_cast<C*>(lua_touserdata(l, -1)));
		luaX_push(l, member);
		return 1;
	};
	std::string getterName("get_");
	getterName.append(name);
	
	std::function<T(C*)> *getInner = static_cast<std::function<T(C*)> *>(
		lua_newuserdata(lua, sizeof(std::function<T(C*)>))
	);
	new(getInner) std::function<T(C*)>;
	*getInner = [member](C* data){return data->*member;};
	lua_pushcclosure(lua, getter, 1);
	lua_setfield(lua, -2, getterName.c_str());
}

template<typename C, typename T>
void luaX_registerClassSetter(lua_State *lua, char const *name, T C::* member) {
	//expects an argument of lightuserdata, type C*, and the value to set arg1->*member to
	auto setter = [] (lua_State *l) {
		//Args list contains: self, val
		auto val = luaX_return<T>(l);
		luaX_getlocal(l, "_data");
		auto memberF = *static_cast<std::function<void(C*, T)>*>(
			lua_touserdata(l, lua_upvalueindex(1))
		);
		memberF(static_cast<C*>(lua_touserdata(l, -1)), val);
		return 0;
	};
	std::string setterName("set_");
	setterName.append(name);
	std::function<void(C*, T)> *setInner = static_cast<std::function<void(C*, T)> *>(
		lua_newuserdata(lua, sizeof(std::function<void(C*, T)>))
	);
	new(setInner) std::function<void(C*, T)>;
	*setInner = [member](C* data, T val){data->*member = val;};
	lua_pushcclosure(lua, setter, 1);
	lua_setfield(lua, -2, setterName.c_str());
}


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
	std::get<I>(ret) = luaX_return<std::tuple_element<I, Tuple>::type>(lua);
	luaX_returntuplefromstackInner<Tuple, I-1>::Fn(lua, ret);
	}
};

template<typename Tuple>
class luaX_returntuplefromstackInner<Tuple, 0> {
public:
	static void Fn(lua_State *lua, Tuple& ret) {
	std::get<0>(ret) = luaX_return<std::tuple_element<0, Tuple>::type>(lua);
	}
};

//TODO specialise for std::tuple and factor out some duplication?

template<typename C, typename... Args>
void luaX_registerClassMethod(lua_State *lua, char const *name, void (C::* member)(Args...)) {
	auto caller = [] (lua_State *l) {
		if(lua_gettop(l) != 1 + sizeof...(Args)) {
			return luaL_error(l, 
					"Bad call to function, %d arguments provided, %d expected (including self)",
					lua_gettop(l),
					1+sizeof...(Args)
				);
		}
		//Arg list contains self, and the arguments for the function
		//we need to replace self with self._data
		lua_getfield(l, 1, "_data");
		lua_insert(l, 2);
		auto args = luaX_returntuplefromstack<C*, Args...>(l);
		auto callerF = *static_cast<std::function<void(C*, Args...)>*>(
			lua_touserdata(l, lua_upvalueindex(1))
		);
		TypeMagic::apply(callerF, args);
		return 0;
	};
	auto *callInner = static_cast<std::function<void(C*, Args...)>*>(
		lua_newuserdata(lua, sizeof(std::function<void(C*, Args...)>))
	);
	new(callInner) std::function<void(C*, Args...)>;
	*callInner = [member](C* data, Args... args) { (data->*member)(args...); };
	lua_pushcclosure(lua, caller, 1);
	lua_setfield(lua, -2, name);
}

template<typename C, typename T, typename... Args>
void luaX_registerClassMethod(lua_State *lua, char const *name, T (C::* member)(Args...)) {
	auto caller = [] (lua_State *l) {
		if(lua_gettop(l) != 1 + sizeof...(Args)) {
			return luaL_error(l, 
					"Bad call to function, %d arguments provided, %d expected (including self)",
					lua_gettop(l),
					1+sizeof...(Args)
				);
		}
		//Arg list contains self, and the arguments for the function
		//we need to replace self with self._data
		lua_getfield(l, 1, "_data");
		lua_insert(l, 2);
		auto args = luaX_returntuplefromstack<C*, Args...>(l);
		auto callerF = *static_cast<std::function<T(C*, Args...)>*>(
			lua_touserdata(l, lua_upvalueindex(1))
		);
		luaX_push<T>(l, TypeMagic::apply(callerF, args));
		return 1;
	};
	auto *callInner = static_cast<std::function<T(C*, Args...)>*>(
		lua_newuserdata(lua, sizeof(std::function<T(C*, Args...)>))
	);
	new(callInner) std::function<T(C*, Args...)>;
	*callInner = [member](C* data, Args... args) { return (data->*member)(args...); };
	lua_pushcclosure(lua, caller, 1);
	lua_setfield(lua, -2, name);
}
