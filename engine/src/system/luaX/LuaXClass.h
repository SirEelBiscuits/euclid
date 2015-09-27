#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include <lua.hpp>

/*
	Used to register a class. Arguements are:
	lua_State*, 
	"name", &Class::Member,...

	If the member is a data member, it will register set_name(x) and get_name()
	otherwise it will register a function to the class which takes exactly the right number of arguments.
	If there are const and non-const overloads of a method, do not list them here.
	Instead use:
		- luaX_registerClassMethodVoid
		- luaX_registerClassMethodVoidConst
		- luaX_registerClassMethodNonVoid
		- luaX_registerClassMethodNonVoidConst
	As appropriate.

	TODO register class const/non-const overloads

	This method returns a luaX_ref which refers to the Class object. see Luaclid.lua for the format of the class.
	Additionally after registration, calling luaX_push(T) with the registered type will push the lua wrapper.	
	*/
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
	luaX_registerClassMethodSingle<T>::Register(lua, name, member);
	luaX_registerClassMethod(lua, args...);
}

template<typename T, typename... Args>
void luaX_registerClassMethod(lua_State *lua, char const *name, T member) {
	luaX_registerClassMethodSingle<T>::Register(lua, name, member);
}

template<typename T>
class luaX_registerClassMethodSingle {
public:
};

template<typename C, typename T>
class luaX_registerClassMethodSingle<T C::*> {
public:
	static void Register(lua_State *lua, char const *name, T C::* member) {
		static_assert(!std::is_function<decltype(member)>::value, "what");
		luaX_registerClassGetter(lua, name, member);
		luaX_registerClassSetter(lua, name, member);
	}
};

template<typename C, typename T>
class luaX_registerClassMethodSingle<T const C::*> {
public:
	static void Register(lua_State *lua, char const *name, T const C::* member) {
		luaX_registerClassGetter(lua, name, member);
	}
};

template<typename C, typename T, typename... Args>
class luaX_registerClassMethodSingle<T (C::*)(Args...)> {
public:
	static void Register(lua_State *lua, char const *name, T (C::* member)(Args...)) {
		luaX_registerClassMethodNonVoid<C, T, 1, Args...>(lua, name, member);
	}
};

template<typename C, typename... RetArgs, typename... Args>
class luaX_registerClassMethodSingle<std::tuple<RetArgs...> (C::*)(Args...)> {
public:
	static void Register(lua_State *lua, char const *name, std::tuple<RetArgs...> (C::* member)(Args...)) {
		luaX_registerClassMethodNonVoid<C, std::tuple<RetArgs...>, sizeof...(RetArgs), Args...>(lua, name, member);
	}
};

template<typename C, typename... Args>
class luaX_registerClassMethodSingle<void (C::*)(Args...)> {
public:
	static void Register(lua_State *lua, char const *name, void (C::* member)(Args...)) {
		luaX_registerClassMethodVoid(lua, name, member);
	}
};

template<typename C, typename... Args>
using voidMemFunPtrConst = void (C::*)(Args...) const;
template<typename T, typename C, typename... Args>
using nonVoidMemFunPtrConst = T (C::*)(Args...) const;

template<typename C, typename... Args>
class luaX_registerClassMethodSingle<voidMemFunPtrConst<C, Args...>> {
public:
	static void Register(lua_State *lua, char const *name, voidMemFunPtrConst<C, Args...> member) {
		luaX_registerClassMethodVoidConst(lua, name, member);
	}
};

template<typename T, typename C, typename... Args>
class luaX_registerClassMethodSingle<nonVoidMemFunPtrConst<T, C, Args...>> {
public:
	static void Register(lua_State *lua, char const *name, nonVoidMemFunPtrConst<T, C, Args...> member) {
		luaX_registerClassMethodNonVoidConst<C, T, 1, Args...>(lua, name, member);
	}
};

template<typename... RetArgs, typename C, typename... Args>
class luaX_registerClassMethodSingle<nonVoidMemFunPtrConst<std::tuple<RetArgs...>, C, Args...>> {
public:
	static void Register(lua_State *lua, char const *name, nonVoidMemFunPtrConst<std::tuple<RetArgs...>, C, Args...> member) {
		luaX_registerClassMethodNonVoidConst<C, std::tuple<RetArgs...>, sizeof...(RetArgs), Args...>(lua, name, member);
	}
};

//weirdly the 'Inner' can be stripped from the name and overloading doesn't break, but that feels like
// it's a bug, or non-conformant, and is non-obvious regardless
template<typename C, typename T, int arity, typename... Args>
void luaX_registerClassMethodNonVoid(lua_State *lua, char const *name, T (C::* member)(Args...)) {
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

///////////////////////////////////
//const

template<typename C, typename... Args>
void luaX_registerClassMethodVoidConst(lua_State *lua, char const *name, voidMemFunPtrConst<C, Args...> member) {
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

//weirdly the 'Inner' can be stripped from the name and overloading doesn't break, but that feels like
// it's a bug, or non-conformant, and is non-obvious regardless
template<typename C, typename T, int arity, typename... Args>
void luaX_registerClassMethodNonVoidConst(lua_State *lua, char const *name, nonVoidMemFunPtrConst<T, C, Args...> member) {
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