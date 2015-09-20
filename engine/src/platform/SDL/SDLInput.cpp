#include "system/Input.h"

#include <SDL.h>

namespace System {
	namespace Input {

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
			ret.button = e.button.button;
			break;
		}

		return ret;
	}

	std::vector<Event> GetEvents() {
		SDL_Event marker;
		marker.type = SDL_USEREVENT;
		marker.user.code = 0xfeedcafe;
		SDL_PushEvent(&marker);

		std::vector<Event> events;
		auto done = false;
		SDL_Event e;
		while(SDL_PollEvent(&e) && !done) {
			switch(e.type) {
				// when we find our marker, we've examined the whole list and can stop
			case SDL_USEREVENT:
				if(e.user.code == 0xfeedcafe) {
					done = true;
					break;
				} else {
					SDL_PushEvent(&e);
				}
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				events.emplace_back(Transform(e));
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
				;
				//these four appear all the damn time and I don't know why
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMAPPED:
			case SDL_CONTROLLERDEVICEREMOVED:
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
				;
				//input we'll want later
			case SDL_MOUSEMOTION:
			case SDL_MOUSEWHEEL:
				break;

				//anything not recognised should be pushed back onto the queue
			default:
				SDL_PushEvent(&e);
				break;
			}
		}

		return events;
	}

	}
}