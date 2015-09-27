#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include <lua.hpp>

template<typename T, typename... Args>
luaX_ref luaX_registerClass(lua_State *lua, Args... args) {
	auto flxr = luaX_typeTable.find(typeid(T));
	if(flxr != luaX_typeTable.end())
		return *flxr->second;
	luaX_getglobal(lua, "CreateNativeClass");
	lua_pcall(lua, 0, 1, 0);
	luaX_registerClassMethod(lua, args...);

	auto lxr = std::make_unique<luaX_ref>(lua);
	auto lxrp = lxr.get();
	luaX_typeTable[typeid(T)] = std::move(lxr);
	return *lxrp;
}

template<typename T, typename... Args>
void luaX_registerClassMethod(lua_State *lua, char const *name, T member, Args... args) {
	luaX_registerClassMethodSingle(lua, name, member);
	luaX_registerClassMethod(lua, args...);
}

template<typename T, typename... Args>
void luaX_registerClassMethod(lua_State *lua, char const *name, T member) {
	luaX_registerClassMethodSingle(lua, name, member);
}

template<
	typename C,
	typename T,
	typename = std::enable_if_t<!std::is_function<T>::value, T>
>
void luaX_registerClassMethodSingle(lua_State *lua, char const *name, T C::* member) {
	luaX_registerClassGetter(lua, name, member);
	luaX_registerClassSetter(lua, name, member);
}

template<
	typename C,
	typename T,
	typename = std::enable_if_t<!std::is_function<T>::value, T>
>
void luaX_registerClassMethodSingle(lua_State *lua, char const *name, T const C::* member) {
	luaX_registerClassGetter(lua, name, member);
}

template<typename C, typename T, typename... Args>
void luaX_registerClassMethodSingle(lua_State *lua, char const *name, T (C::* member)(Args...)) {
	luaX_registerClassMethodInner<C, T, 1, Args...>(lua, name, member);
}
template<typename C, typename... RetArgs, typename... Args>
void luaX_registerClassMethodSingle(lua_State *lua, char const *name, std::tuple<RetArgs...> (C::* member)(Args...)) {
	luaX_registerClassMethodInner<C, std::tuple<RetArgs...>, sizeof...(RetArgs), Args...>(lua, name, member);
}

template<typename C, typename... Args>
void luaX_registerClassMethodSingle(lua_State *lua, char const *name, void (C::* member)(Args...)) {
	luaX_registerClassMethodVoid(lua, name, member);
}

//weirdly the 'Inner' can be stripped from the name and overloading doesn't break, but that feels like
// it's a bug, or non-conformant, and is non-obvious regardless
template<typename C, typename T, int arity, typename... Args>
void luaX_registerClassMethodInner(lua_State *lua, char const *name, T (C::* member)(Args...)) {
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
		luaX_push(l, TypeMagic::apply(callerF, args));
		return arity;
	};
	auto *callInner = static_cast<std::function<T(C*, Args...)>*>(
		lua_newuserdata(lua, sizeof(std::function<T(C*, Args...)>))
	);
	new(callInner) std::function<T(C*, Args...)>;
	*callInner = [member](C* data, Args... args) { return (data->*member)(args...); };
	lua_pushcclosure(lua, caller, 1);
	lua_setfield(lua, -2, name);
}

template<typename C, typename... Args>
void luaX_registerClassMethodVoid(lua_State *lua, char const *name, void (C::* member)(Args...)) {
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