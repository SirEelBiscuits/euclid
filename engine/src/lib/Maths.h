#include "PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB

POST_STD_LIB

namespace TypeMagic {

	template<unsigned power>
	class pow {
	public:
		template<typename T, typename impl = pow<power - 1>>
		static constexpr auto calculate(T const &in) {
			return in * impl::calculate(in);
		}
	};

	template<>
	class pow<1> {
	public:
		template<typename T>
		static constexpr T calculate(T const &in) {
			return in;
		}
	};

	template<typename T>
	static constexpr auto invert(T const &in) {
		return in/in/in;
	}
}

#define X_TO_POWER(x, power) (TypeMagic::pow<power>::calculate(x))
#define X_TO_POWER_TYPE(x, power) decltype(X_TO_POWER(x, power))
#define SAFISH_INVERT(in) (TypeMagic::invert(in))
#define INVERSE_TYPE(in) decltype(SAFISH_INVERT(in))
