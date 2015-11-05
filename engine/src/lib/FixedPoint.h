#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cinttypes>
#include <ostream>
POST_STD_LIB

class FixedPoint {
public:
	constexpr FixedPoint();
	constexpr explicit FixedPoint(int8_t in);
	constexpr explicit FixedPoint(int16_t in);
	constexpr explicit FixedPoint(int32_t in);
	constexpr explicit FixedPoint(int64_t in);
	constexpr explicit FixedPoint(uint8_t in);
	constexpr explicit FixedPoint(uint16_t in);
	constexpr explicit FixedPoint(uint32_t in);
	constexpr explicit FixedPoint(uint64_t in);
	constexpr explicit FixedPoint(float in);

	enum FixedRaw{RAW};
	constexpr FixedPoint(FixedRaw, int32_t in);
	
	explicit operator int8_t();
	explicit operator int16_t();
	explicit operator int32_t();
	explicit operator int64_t();
	explicit operator uint8_t();
	explicit operator uint16_t();
	explicit operator uint32_t();
	explicit operator uint64_t();
	explicit operator float();

	FixedPoint &operator+=(FixedPoint other);
	FixedPoint &operator-=(FixedPoint other);
	FixedPoint &operator*=(FixedPoint other);
	FixedPoint &operator/=(FixedPoint other);

	constexpr int32_t _getStore() { return store; }

	static int   const shift;
	static int   const shiftMult;
	static float const shiftMultF;
private:
	int32_t store;
};

constexpr FixedPoint operator+ (FixedPoint in);
constexpr FixedPoint operator- (FixedPoint in);
constexpr FixedPoint operator+ (FixedPoint left, FixedPoint right);
constexpr FixedPoint operator- (FixedPoint left, FixedPoint right);
constexpr FixedPoint operator* (FixedPoint left, FixedPoint right);
constexpr FixedPoint operator/ (FixedPoint left, FixedPoint right);
/*constexpr*/ bool       operator< (FixedPoint left, FixedPoint right);
constexpr bool       operator> (FixedPoint left, FixedPoint right);
constexpr bool       operator==(FixedPoint left, FixedPoint right);
constexpr bool       operator<=(FixedPoint left, FixedPoint right);
constexpr bool       operator>=(FixedPoint left, FixedPoint right);
constexpr bool       operator!=(FixedPoint left, FixedPoint right);

/*constexpr*/ FixedPoint operator "" _fp(unsigned long long in);
/*constexpr*/ FixedPoint operator "" _fp(long double in);

std::ostream &operator<<(std::ostream& os, FixedPoint fp);

using Fixed16x16 = FixedPoint;
