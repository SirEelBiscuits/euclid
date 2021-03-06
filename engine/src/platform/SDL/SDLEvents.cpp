#include "system/Events.h"

#include <SDL.h>

namespace System {
	static std::function<void(Events::Types, void*, void*)> EventCallback;

	namespace Input {
		void Initialise() {
			SetTextEditingMode(false);
		}

		void SetMouseShowing(bool show) {
			SDL_SetRelativeMouseMode(show? SDL_FALSE : SDL_TRUE);
		}

		bool GetMouseShowing() {
			return SDL_GetRelativeMouseMode() == SDL_FALSE;
		}

		bool ReturnOnKeyInput() {
			SDL_Event e;
			while(true) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
					case SDL_KEYDOWN:
						return e.key.keysym.sym == SDLK_ESCAPE;
					}
				}
				SDL_Delay(0);
			}
		}

		Event Transform(SDL_Event e) {
			Event ret;

			switch(e.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				ret.type = e.type == SDL_KEYDOWN? EventType::KeyDown : EventType::KeyUp;
				ret.repeat = !!e.key.repeat;
				ret.key = e.key.keysym.sym;
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				ret.type = e.type == SDL_MOUSEBUTTONDOWN? EventType::MouseDown : EventType::MouseUp;
				ret.repeat = false;
				switch(e.button.button) {
				case SDL_BUTTON_LEFT:
					ret.button = (int)MouseButtons::Left;
					break;
				case SDL_BUTTON_RIGHT:
					ret.button = (int)MouseButtons::Right;
					break;
				case SDL_BUTTON_MIDDLE:
					ret.button = (int)MouseButtons::Middle;
					break;
				default:
					ret.button = (int)MouseButtons::UnknownStart + e.button.button;
				}
				break;
			case SDL_MOUSEMOTION:
				ret.type = EventType::MouseMove;
				ret.mouseMovX = e.motion.x;
				ret.mouseMovY = e.motion.y;
				ret.mouseMovXRel = e.motion.xrel;
				ret.mouseMovYRel = e.motion.yrel;
				break;
			case SDL_MOUSEWHEEL:
				ret.type = EventType::MouseScroll;
				if(e.wheel.y > 0)
					ret.button = (int)MouseButtons::ScrollUp;
				else if(e.wheel.y < 0)
					ret.button = (int)MouseButtons::ScrollDown;
				break;
			default:
				ASSERT(false);
			}

			return ret;
		}

		static int const DEBUG_START_RECORD_KEY = SDLK_F5;
		static int const DEBUG_STOP_RECORD_KEY  = SDLK_F6;
		static int const DEBUG_LOOP_RECORD_KEY  = SDLK_F7;
		static int const DEBUG_RENDERING_KEY    = SDLK_F4;

		static bool LShiftDown = false;
		static bool RShiftDown = false;
		static bool LCtrlDown  = false;
		static bool RCtrlDown  = false;
		
		static bool textEditing{false};

		std::vector<Event> GetEvents() {
			std::vector<Event> events;
			auto done = false;
			SDL_Event e;
			while(SDL_PollEvent(&e) && !done) {
				switch(e.type) {
				case SDL_KEYDOWN:
#ifndef EUCLID_FINAL_RELEASE
					switch(e.key.keysym.sym) {
					case DEBUG_START_RECORD_KEY:
						EventCallback(Events::Types::InputLoopStart, nullptr, nullptr);
						break;
					case DEBUG_STOP_RECORD_KEY:
						EventCallback(Events::Types::InputLoopStop, nullptr, nullptr);
						break;
					case DEBUG_LOOP_RECORD_KEY:
						EventCallback(Events::Types::InputLoopMarkEnd, nullptr, nullptr);
						break;
					case DEBUG_RENDERING_KEY:
						EventCallback(Events::Types::DebugRenderingStart, nullptr, nullptr);
						break;
					default:;
					}
#endif
				//FALLTHROUGH
				case SDL_KEYUP:
#ifdef EUCLID_FINAL_RELEASE
					if(e.key.keysym.sym == DEBUG_START_RECORD_KEY
						|| e.key.keysym.sym == DEBUG_STOP_RECORD_KEY
						|| e.key.keysym.sym == DEBUG_LOOP_RECORD_KEY
					)
						break;
#endif
				//FALLTHROUGH
					switch(e.key.keysym.sym) {
					case SDLK_LCTRL:
						LCtrlDown = e.type == SDL_KEYDOWN;
						break;
					case SDLK_RCTRL:
						RCtrlDown = e.type == SDL_KEYDOWN;
						break;
					case SDLK_LSHIFT:
						LShiftDown = e.type == SDL_KEYDOWN;
						break;
					case SDLK_RSHIFT:
						RShiftDown = e.type == SDL_KEYDOWN;
						break;
					default:
						break;
					}
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEMOTION:
				case SDL_MOUSEWHEEL:
					events.emplace_back(Transform(e));
					break;

				case SDL_TEXTINPUT:
					ASSERT(textEditing);
					if(textEditing){
						System::Input::Event ret;
						ret.textInput = e.text.text;
						ret.type = EventType::TextInput;
						events.emplace_back(ret);
					}
					break;

					//input events we recognise, but aren't interested it
				case SDL_CONTROLLERAXISMOTION:
				case SDL_CONTROLLERBUTTONDOWN:
				case SDL_CONTROLLERBUTTONUP:
				case SDL_DOLLARGESTURE:
				case SDL_DOLLARRECORD:
				case SDL_FINGERMOTION:
				case SDL_FINGERDOWN:
				case SDL_FINGERUP:
				case SDL_JOYAXISMOTION:
				case SDL_JOYBALLMOTION:
				case SDL_JOYHATMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
				case SDL_MULTIGESTURE:
					break;

				/////////////////////////////////////////////
				// Callback events
				case SDL_QUIT:
					EventCallback(Events::Types::Quit, nullptr, nullptr);
					break;

				case SDL_APP_TERMINATING:
				case SDL_APP_LOWMEMORY:
				case SDL_APP_WILLENTERBACKGROUND:
				case SDL_APP_DIDENTERBACKGROUND:
				case SDL_APP_WILLENTERFOREGROUND:
				case SDL_APP_DIDENTERFOREGROUND:
				case SDL_WINDOWEVENT:
				case SDL_SYSWMEVENT:
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED:
				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
				case SDL_CONTROLLERDEVICEREMAPPED:
				case SDL_CLIPBOARDUPDATE:
				case SDL_DROPFILE:
				case SDL_RENDER_TARGETS_RESET:
					EventCallback(Events::Types::Unknown, (void*)&e, nullptr);
					break;

				default:
					//SDL_PushEvent(&e);
					break;
				}
			}
			return events;
		}

		bool IsShiftDown() { 
			return LShiftDown || RShiftDown;
		}

		bool IsCtrlDown() {
			return LCtrlDown || RCtrlDown;
		}

		void SetTextEditingMode(bool SetOn) {
			textEditing = SetOn;
			if(SetOn)
				SDL_StartTextInput();
			else
				SDL_StopTextInput();
		}
	}

	namespace Events {		
		void SetHandlers(std::function<void(Types, void*, void*)> callback) {
			EventCallback = callback;
		}
	}
}