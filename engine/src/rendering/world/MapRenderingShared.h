#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
POST_STD_LIB

#include "lib/BasicTypes.h"

namespace World {
	class Sector;
}

namespace Rendering {

	namespace World {

		struct View {
			PositionVec3      eye    {};
			::World::Sector * sector {nullptr};
			TransformMat2     forward{};

			PositionVec2 ToViewSpace(PositionVec2 in) const {
				return forward * (in - AsVec2(eye));
			}

			PositionVec3 ToViewSpace(PositionVec3 in) const {
				auto p3 = in - eye;
				auto r = forward * AsVec2(p3);
				return PositionVec3(r.x, r.y, p3.z);
			}
		};
	}
}
