#include "FixedPoint.h"

PRE_STD_LIB
#include <ostream>
POST_STD_LIB

FixedPoint & FixedPoint::operator+=(FixedPoint other) {
	return *this = *this + other;
}

FixedPoint & FixedPoint::operator-=(FixedPoint other) {
	return *this = *this - other;
}

FixedPoint & FixedPoint::operator*=(FixedPoint other) {
	return *this = *this * other;
}

FixedPoint & FixedPoint::operator/=(FixedPoint other) {
	return *this = *this / other;
}

std::ostream &operator<<(std::ostream& os, FixedPoint fp) {
	return os << static_cast<float>(fp);
}
