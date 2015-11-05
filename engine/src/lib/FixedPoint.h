#include "lib/PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <cinttypes>
#include <ostream>
POST_STD_LIB

class FixedPoint {
public:
	constexpr FixedPoint()                     : store(0) {};
	constexpr explicit FixedPoint(int8_t in)   : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(int16_t in)  : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(int32_t in)  : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(int64_t in)  : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(uint8_t in)  : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(uint16_t in) : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(uint32_t in) : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(uint64_t in) : store((int32_t)in << shift) {}
	constexpr explicit FixedPoint(float in)    : store((int32_t)(in *shiftMult)) {}

	enum FixedRaw{RAW};
	constexpr FixedPoint(FixedRaw, int32_t in) : store(in) {}
	
	constexpr explicit operator int8_t() const { return static_cast<int8_t>(store >> shift); }
	explicit operator int16_t()          const { return static_cast<int16_t>(store >> shift); }
	explicit operator int32_t()          const { return static_cast<int32_t>(store >> shift); }
	explicit operator int64_t()          const { return static_cast<int64_t>(store >> shift); }
	explicit operator uint8_t()          const { return static_cast<uint8_t>(store >> shift); }
	explicit operator uint16_t()         const { return static_cast<uint16_t>(store >> shift); }
	explicit operator uint32_t()         const { return static_cast<uint32_t>(store >> shift); }
	explicit operator uint64_t()         const { return static_cast<uint64_t>(store >> shift); }
	explicit operator float()            const { return static_cast<float>(store /shiftMultF); }

	FixedPoint &operator+=(FixedPoint other);
	FixedPoint &operator-=(FixedPoint other);
	FixedPoint &operator*=(FixedPoint other);
	FixedPoint &operator/=(FixedPoint other);

	///if you call this, you better know wtf you are doing
	constexpr int32_t _getStore() { return store; }

	static int   constexpr shift = 16;
	static int   constexpr shiftMult = 1 << shift;
	static float constexpr shiftMultF = (float)shiftMult;
private:
	int32_t store;
};

constexpr FixedPoint operator+ (FixedPoint in) { return in; };
constexpr FixedPoint operator- (FixedPoint in) { return FixedPoint(FixedPoint::RAW, -in._getStore()); }
constexpr FixedPoint operator+ (FixedPoint left, FixedPoint right) { return FixedPoint(FixedPoint::RAW, left._getStore() + right._getStore()); }
constexpr FixedPoint operator- (FixedPoint left, FixedPoint right) { return FixedPoint(FixedPoint::RAW, left._getStore() - right._getStore()); }
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
			(
				(static_cast<int64_t>(left._getStore()) << (FixedPoint::shift*2)) 
					/ static_cast<int64_t>(right._getStore())
			) >> FixedPoint::shift
		)
	);
}
constexpr bool       operator< (FixedPoint left, FixedPoint right) { return left._getStore() < right._getStore(); }
constexpr bool       operator> (FixedPoint left, FixedPoint right) { return left._getStore() > right._getStore(); }
constexpr bool       operator==(FixedPoint left, FixedPoint right) { return left._getStore() == right._getStore(); }
constexpr bool       operator<=(FixedPoint left, FixedPoint right) { return left._getStore() <= right._getStore(); }
constexpr bool       operator>=(FixedPoint left, FixedPoint right) { return left._getStore() >= right._getStore(); }
constexpr bool       operator!=(FixedPoint left, FixedPoint right) { return left._getStore() != right._getStore(); }

constexpr FixedPoint operator "" _fp(unsigned long long in) { return FixedPoint(static_cast<int32_t>(in)); }
constexpr FixedPoint operator "" _fp(long double in) { return FixedPoint(static_cast<float>(in)); }

std::ostream &operator<<(std::ostream& os, FixedPoint fp);

using Fixed16x16 = FixedPoint;
