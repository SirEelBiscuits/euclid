#include "PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <utility>
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

	//from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3915.pdf

	template<typename F, typename Tuple, size_t... I>
	decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>) {
		return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
	}

	template<typename F, typename Tuple>
	decltype(auto) apply(F&& f, Tuple&& t) {
		using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
		return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
	}

	template<typename T>
	struct arity {
		static int const value = 1;
	};

	template<>
	struct arity<void> {
		static int const value = 0;
	};

	template<typename... Args>
	struct arity<std::tuple<Args...>> {
		static int const value = sizeof...(Args);
	};
}