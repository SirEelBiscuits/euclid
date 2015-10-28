#include "PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB

POST_STD_LIB

#include "TypeMagic.h"

#define X_TO_POWER(x, power) (TypeMagic::pow<power>::calculate(x))
#define X_TO_POWER_TYPE(x, power) decltype(X_TO_POWER(x, power))
#define SAFISH_INVERT(in) (TypeMagic::invert(in))
#define INVERSE_TYPE(in) decltype(SAFISH_INVERT(in))

#define PI 3.141592653f

namespace Maths {
	template<typename T, typename S>
	constexpr auto min(T a, S b) {
		return a > b ? b : a;
	}

	template<typename T, typename S>
	constexpr auto max(T a, S b) {
		return a > b ? a : b;
	}

	template<typename T, typename S>
	constexpr auto clamp(T a, S lower, S upper) {
		return min(max(a, lower), upper);
	}

	template<typename T, typename S, typename R>
	constexpr auto interp(T start, S end, R alpha) {
		return start + (end - start) * alpha;
	}

	template<typename T, typename S, typename R>
	constexpr auto reverseInterp(T start, S end, R target) {
		return (target - start) / (end - start);
	}

	/*constexpr*/ int nextBiggestPow2(int val);
}