#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <vector>
#include <algorithm>
#include <utility>
POST_STD_LIB

namespace Rendering {
	class RenderList {
	public:
		using RenderFunction = void (*)(void*);
		using pairType = std::pair<void*, RenderFunction>;

		void Register(void *rb, RenderFunction rf) {
			if(_registry.size() == _registry.capacity())
				Compact();
			_registry.emplace_back(pairType{rb, rf});
		}

		void Deregister(void *rb) {
			for(auto &p : _registry)
				p.first = p.first == rb ? nullptr : p.first;
		}

		void Render() {
			for(auto &p : _registry)
				if(nullptr != p.first)
					p.second(p.first);
		}

		void Compact() {
			std::remove_if(
				_registry.begin(), _registry.end(),
				[](pairType &p){ return nullptr == p.first; }
			);
		}

	private:
		std::vector<pairType> _registry{};
	};
}
