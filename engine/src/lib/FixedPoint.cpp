#include "FixedPoint.h"

PRE_STD_LIB
#include <ostream>
POST_STD_LIB

int const FixedPoint::shift = 16;
int const FixedPoint::shiftMult = 1 << FixedPoint::shift;
float const FixedPoint::shiftMultF = (float)shiftMult;

constexpr FixedPoint::FixedPoint() 
	: store(0)
{}

constexpr FixedPoint::FixedPoint(int8_t in) 
	: store((int32_t)in << shift)
{}

constexpr FixedPoint::FixedPoint(int16_t in) 
	: store((int32_t)in << shift)
{}

constexpr FixedPoint::FixedPoint(int32_t in) 
	: store(in << shift)
{}

constexpr FixedPoint::FixedPoint(int64_t in) 
	: store((int32_t)in << shift)
{}

constexpr FixedPoint::FixedPoint(uint8_t in) 
	: store((int32_t)in << shift)
{}

constexpr FixedPoint::FixedPoint(uint16_t in) 
	: store((int32_t)in << shift)
{}

constexpr FixedPoint::FixedPoint(uint32_t in) 
	: store(in << shift)
{}

constexpr FixedPoint::FixedPoint(uint64_t in) 
	: store((int32_t)in << shift)
{}

constexpr FixedPoint::FixedPoint(float in) 
	: store((int32_t)(in * shiftMult))
{}

constexpr FixedPoint::FixedPoint(FixedRaw, int32_t in)
	: store(in)
{}

FixedPoint::operator int8_t() {
	return static_cast<int8_t>(store >> shift);
}

FixedPoint::operator int16_t() {
	return static_cast<int16_t>(store >> shift);
}

FixedPoint::operator int32_t() {
	return store >> shift;
}

FixedPoint::operator int64_t() {
	return static_cast<int64_t>(store >> shift);
}

FixedPoint::operator uint8_t() {
	return static_cast<uint8_t>(store >> shift);
}

FixedPoint::operator uint16_t() {
	return static_cast<uint16_t>(store >> shift);
}

FixedPoint::operator uint32_t() {
	return store >> shift;
}

FixedPoint::operator uint64_t() {
	return static_cast<uint64_t>(store >> shift);
}

FixedPoint::operator float() {
	return store / shiftMultF;
}

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

constexpr FixedPoint operator+ (FixedPoint in) {
	return in;
}

constexpr FixedPoint operator- (FixedPoint in) {
	return FixedPoint(FixedPoint::RAW, -in._getStore());
}

constexpr FixedPoint operator+ (FixedPoint left, FixedPoint right) {
	return FixedPoint(FixedPoint::RAW, left._getStore() + right._getStore());
}

constexpr FixedPoint operator- (FixedPoint left, FixedPoint right) {
	return FixedPoint(FixedPoint::RAW, left._getStore() - right._getStore());
}

constexpr FixedPoint operator* (FixedPoint left, FixedPoint right) {
	return FixedPoint(FixedPoint::RAW, 
		static_cast<int32_t>(
			(static_cast<int64_t>(left._getStore()) * static_cast<int64_t>(right._getStore())
				>> FixedPoint::shift)
		)
	);
}

constexpr FixedPoint operator/ (FixedPoint left, FixedPoint right) {
	return FixedPoint(FixedPoint::RAW,
		static_cast<int32_t>(
			static_cast<int64_t>(left._getStore()) << (FixedPoint::shift*2) 
				/ static_cast<int64_t>(right._getStore()) >> FixedPoint::shift
		)
	);
}

/*constexpr*/ bool operator< (FixedPoint left, FixedPoint right) {
	return left._getStore() < right._getStore();
}

constexpr bool operator> (FixedPoint left, FixedPoint right) {
	return left._getStore() > right._getStore();
}

constexpr bool operator==(FixedPoint left, FixedPoint right) {
	return left._getStore() == right._getStore();
}

constexpr bool operator<=(FixedPoint left, FixedPoint right) {
	return left._getStore() <= right._getStore();
}

constexpr bool operator>=(FixedPoint left, FixedPoint right) {
	return left._getStore() >= right._getStore();
}

constexpr bool operator!=(FixedPoint left, FixedPoint right) {
	return left._getStore() != right._getStore();
}

/*constexpr*/ FixedPoint operator "" _fp(unsigned long long in) {
	return FixedPoint(static_cast<int32_t>(in));
}

/*constexpr*/ FixedPoint operator "" _fp(long double in) {
	return FixedPoint(static_cast<float>(in));
}

std::ostream &operator<<(std::ostream& os, FixedPoint fp) {
	return os << static_cast<float>(fp);
}
