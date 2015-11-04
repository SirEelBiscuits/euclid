#include "Controls.h"

PRE_STD_LIB
#include <string>
POST_STD_LIB

#include "lib/Maths.h"

namespace System {
	namespace Controls {
		void Config::HandleInput(Input::Event e) {
			for(auto &i : inputList) 
				i->HandleEvent(e);
		}

		bool KeyIsReasonable(char c) {
			return true;
		}

		void Config::LoadControls(lua_State *lua) {
			if(lua_istable(lua, -1) == false)
				return; //todo error or something?

			auto id = 1;
			do {
				auto top = lua_gettop(lua);
				lua_geti(lua, -1, id);
				auto pushedItems = 1;
				if(lua_isnil(lua, -1)) {
					lua_pop(lua, pushedItems);
					break;
				}

				auto controlType = luaX_returnlocal<std::string>(lua, "ControlType");

				if(controlType.compare("button") == 0) {
					pushedItems += luaX_getlocal(lua, "Name");
					if(lua_type(lua, -1) != LUA_TSTRING) {
						lua_pop(lua, pushedItems);
						continue;
					}
					auto Name = luaX_return<std::string>(lua);
					--pushedItems;

					pushedItems += luaX_getlocal(lua, "Key");
					auto Key = 0;
					switch(lua_type(lua, -1)) {
					case LUA_TSTRING: {
							auto str = luaX_return<std::string>(lua);
							--pushedItems;
							if(str.length() != 1) {
								lua_pop(lua, pushedItems);
								ASSERT(false); //does this work right?
								continue;
							}
							if(KeyIsReasonable(str[0]))
								Key = str[0];
						}
						break;
					case LUA_TNUMBER: {
							Key = luaX_return<int>(lua); //if the number is rubbish, that's your problem
							--pushedItems;
						}
						break;
					default:
						lua_pop(lua, pushedItems);
						ASSERT(false); //just once I need to check this continue goes to the right place!
						continue;
					}
					
					inputList.emplace_back(std::make_unique<Button>(Name, Key));
				}

				else if(controlType.compare("axis") == 0) {
					pushedItems += luaX_getlocal(lua, "Name");
					if(lua_type(lua, -1) != LUA_TSTRING) {
						lua_pop(lua, pushedItems);
						continue;
					}
					auto Name = luaX_return<std::string>(lua);
					--pushedItems;

					pushedItems += luaX_getlocal(lua, "PosKey");
					auto PosKey = 0;
					switch(lua_type(lua, -1)) {
					case LUA_TSTRING: {
							auto str = luaX_return<std::string>(lua);
							--pushedItems;
							if(str.length() != 1) {
								lua_pop(lua, pushedItems);
								ASSERT(false); //does this work right?
								continue;
							}
							if(KeyIsReasonable(str[0]))
								PosKey = str[0];
						}
						break;
					case LUA_TNUMBER: {
							PosKey = luaX_return<int>(lua); //if the number is rubbish, that's your problem
							--pushedItems;
						}
						break;
					default:
						lua_pop(lua, pushedItems);
						ASSERT(false); //just once I need to check this continue goes to the right place!
						continue;
					}

					pushedItems += luaX_getlocal(lua, "NegKey");
					auto NegKey = 0;
					switch(lua_type(lua, -1)) {
					case LUA_TSTRING: {
							auto str = luaX_return<std::string>(lua);
							--pushedItems;
							if(str.length() != 1) {
								lua_pop(lua, pushedItems);
								ASSERT(false); //does this work right?
								continue;
							}
							if(KeyIsReasonable(str[0]))
								NegKey = str[0];
							break;
						}
					case LUA_TNUMBER: {
							NegKey = luaX_return<int>(lua); //if the number is rubbish, that's your problem
							--pushedItems;
						}
					default:
						lua_pop(lua, pushedItems);
						ASSERT(false); //just once I need to check this continue goes to the right place!
						continue;
					}

					inputList.emplace_back(std::make_unique<Axis>(Name, PosKey, NegKey));
				}

				else if(controlType.compare("mouse") == 0) {
					pushedItems += luaX_getlocal(lua, "Name");
					if(lua_type(lua, -1) != LUA_TSTRING) {
						lua_pop(lua, pushedItems);
						continue;
					}
					auto name = luaX_return<std::string>(lua);
					--pushedItems;

					AxisDirection axis;
					pushedItems += luaX_getlocal(lua, "axis");
					if(lua_type(lua, -1) != LUA_TSTRING) {
						lua_pop(lua, pushedItems);
						continue;
					}
					auto axisString = luaX_return<std::string>(lua);
					--pushedItems;
					if(axisString.compare("x") == 0)
						axis = AxisDirection::x;
					else if(axisString.compare("y") == 0)
						axis = AxisDirection::y;
					else {
						lua_pop(lua, pushedItems);
						continue;
					}

					pushedItems += luaX_getlocal(lua, "relative");
					if(lua_type(lua, -1) != LUA_TBOOLEAN) {
						lua_pop(lua, pushedItems);
						continue;
					}
					auto relative = luaX_return<bool>(lua);
					--pushedItems;

					pushedItems += luaX_getlocal(lua, "scale");
					auto scale = 1.f;
					if(lua_type(lua, -1) == LUA_TNUMBER) {
						scale = luaX_return<float>(lua);
						--pushedItems;
					} else {
						lua_pop(lua, 1);
						--pushedItems;
					}

					pushedItems += luaX_getlocal(lua, "maxMagnitude");
					auto maxMag = FLT_MAX;
					if(lua_type(lua, -1) == LUA_TNUMBER) {
						maxMag = luaX_return<float>(lua);
						--pushedItems;
					} else {
						lua_pop(lua, 1);
						--pushedItems;
					}

					if(relative)
						inputList.emplace_back(std::make_unique<MouseAxisRel>(name, axis, scale, maxMag));
					else
						inputList.emplace_back(std::make_unique<MouseAxis>(name, axis, scale, maxMag));
				}

				lua_pop(lua, pushedItems);
				ASSERT(lua_gettop(lua) == top);
				++id;
			} while(true);
		}

		void Config::PushInputInfo(lua_State *lua) {
			luaX_push(lua, luaX_emptytable(0, inputList.size()));
			for(auto &i : inputList) {
				i->SetValue(lua);
			}
		}

		bool isMouseEvent(Input::Event e) {
			switch(e.type) {
			case System::Input::EventType::MouseDown:
			case System::Input::EventType::MouseUp:
				return true;
			case System::Input::EventType::KeyDown:
			case System::Input::EventType::KeyUp:
				return false;
			default:
				ASSERT(false);
			}
			return false;
		}

		Button::Button(std::string name, int key)
			: name(name)
			, key(key)
			, state(UnPressed)
			, newlyPressed(false)
			, newlyReleased(false)
		{}

		void Button::HandleEvent(Input::Event e) {
			if(e.key != key || isMouseEvent(e) || e.repeat)
				return;

			switch(e.type) {
			case System::Input::EventType::KeyDown:
				 newlyPressed = state == UnPressed;
				 state = Pressed;
				 break;
			case System::Input::EventType::KeyUp:
				newlyReleased = state == Pressed;
				state = UnPressed;
				break;
			default:
				ASSERT(false);
			}
		}

		void Button::SetValue(lua_State *lua) {
			luaX_push(lua, luaX_emptytable(0, 3));
			lua_pushboolean(lua, state == Pressed);
			lua_setfield(lua, -2, "isDown");
			luaX_setlocal(lua, "pressed", newlyPressed);
			luaX_setlocal(lua, "released", newlyReleased);
			lua_setfield(lua, -2, name.c_str());

			newlyPressed = false;
			newlyReleased = false;
		}

		Axis::Axis(std::string name, int posKey, int negKey) 
			: name(name)
			, posKey(posKey)
			, negKey(negKey)
			, state(0)
		{}

		void Axis::HandleEvent(Input::Event e) {
			if((e.key != posKey && e.key != negKey) || isMouseEvent(e) || e.repeat)
				return;

			switch(e.type) {
			case System::Input::EventType::KeyDown:
				state += (e.key == posKey? 1 : -1);
				break;
			case System::Input::EventType::KeyUp:
				state += (e.key == posKey? -1 : 1);
				break;
			default:
				ASSERT(false);
			}
		}

		void Axis::SetValue(lua_State *lua) {
			lua_pushnumber(lua, state);
			lua_setfield(lua, -2, name.c_str());
		}

		MouseAxis::MouseAxis(std::string name, AxisDirection axis, float scale, float maxMagnitude)
			: name(name)
			, axis(axis)
			, scale(scale)
			, maxMagnitude(maxMagnitude)
			, state(0)
		{}

		void MouseAxis::HandleEvent(Input::Event e) {
			if(e.type != System::Input::EventType::MouseMove)
				return;

			switch(axis) {
			case AxisDirection::x:
				state = e.mouseMovX;
				break;
			case AxisDirection::y:
				state = e.mouseMovY;
				break;
			default:
				ASSERT(false); //well, you never know
			}
		}

		void MouseAxis::SetValue(lua_State *lua) {
			lua_pushnumber(lua, state);
			lua_setfield(lua, -2, name.c_str());
		}

		MouseAxisRel::MouseAxisRel(std::string name, AxisDirection axis, float scale, float maxMagnitude)
			: name(name)
			, axis(axis)
			, scale(scale)
			, maxMagnitude(maxMagnitude)
			, state(0)
		{}

		void MouseAxisRel::HandleEvent(Input::Event e) {
			if(e.type != System::Input::EventType::MouseMove)
				return;

			switch(axis) {
			case AxisDirection::x:
				state += e.mouseMovXRel * scale;
				break;
			case AxisDirection::y:
				state += e.mouseMovYRel * scale;
				break;
			default:
				ASSERT(false); //well, you never know
			}
		}

		void MouseAxisRel::SetValue(lua_State *lua) {
			state = Maths::clamp(state, -maxMagnitude, maxMagnitude);
			lua_pushnumber(lua, state);
			lua_setfield(lua, -2, name.c_str());

			state = 0;
		}
	}
}
