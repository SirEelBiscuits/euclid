#include "BasicTypes.h"

#include <cmath>

PositionVec2 AsVec2(PositionVec3 const & vec) {
	return PositionVec2(vec.x, vec.y);
}

TransformMat2 RotationMatrix(btStorageType angle) {
	auto c = std::cos(angle);
	auto s = std::sin(angle);
	return TransformMat2(
			c, s,
			-s, c
		);
}
