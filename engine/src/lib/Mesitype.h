#include "PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB
#include <string>
#include <ostream>
POST_STD_LIB

// TODO add copyright notice?

/**
 * @brief Main class to store SI types
 *
 * @param T storage type parameter
 * @param t_m number of dimensions of meters, e.g., t_m == 2 => m^2
 * @param t_s similar to t_m, but seconds
 * @param t_kg similar to t_m, but kilograms
 *
 * This class is to enforce compile-time checking, and where possible,
 * compile-time calculation of SI values using constexpr.
 *
 * Note: MESI_LITERAL_TYPE may be defined to set the storage type
 * used by the operator literal overloads
 *
 * @author Jameson Thatcher (a.k.a. SirEel)
 *
 */
template<typename T, int t_m, int t_s, int t_kg>
struct MesiType {
	using BaseType = T;
  T val;

  constexpr MesiType()
  {}

  constexpr explicit MesiType(T const in)
    :val(in)
  {}

  constexpr MesiType(MesiType const& in)
    :val(in.val)
  {}

	//explicit cast operators.
	// we don't want implicit casts as this may shortcut the compile time type enforcement

	explicit operator T() const {
		return val;
	}

  /**
   * getUnit will get a SI-style unit string for this class
   */
  static std::string const& getUnit() {
    static std::string s_unitString("");
    if( s_unitString.size() > 0 )
      return  s_unitString;

REMOVE_VS_WARNING(4127) //Conditional is constant
    if( t_m == 1 )
      s_unitString += "m ";
    else if( t_m != 0 )
      s_unitString += "m^"
        + std::to_string(static_cast<long long>(t_m)) + " ";
    if( t_s == 1 )
      s_unitString += "s ";
    else if( t_s != 0 )
      s_unitString += "s^"
        + std::to_string(static_cast<long long>(t_s)) + " ";
    if( t_kg == 1 )
      s_unitString += "kg ";
    else if( t_kg != 0 )
      s_unitString += "kg^"
        + std::to_string(static_cast<long long>(t_kg)) + " ";
ALLOW_VS_WARNING(4127)
    s_unitString = s_unitString.substr(0, s_unitString.size() - 1);
    return s_unitString;
  }

  MesiType<T, t_m, t_s, t_kg>& operator+=(MesiType<T, t_m, t_s, t_kg> const& rhs) {
    return (*this) = (*this) + rhs;
  }

  MesiType<T, t_m, t_s, t_kg>& operator-=(MesiType<T, t_m, t_s, t_kg> const& rhs) {
    return (*this) = (*this) - rhs;
  }

  MesiType<T, t_m, t_s, t_kg>& operator*=(T const& rhs) {
    return (*this) = (*this) * rhs;
  }

  MesiType<T, t_m, t_s, t_kg>& operator /=(T const& rhs) {
    return (*this) = (*this) / rhs;
  }
};

/*
 * Arithmatic operators for combining SI values.
 */

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator+(
    MesiType<T, t_m, t_s, t_kg> const& left,
    MesiType<T, t_m, t_s, t_kg> const& right
) {
  return MesiType<T, t_m, t_s, t_kg>(left.val + right.val);
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator-(
    MesiType<T, t_m, t_s, t_kg> const& left,
    MesiType<T, t_m, t_s, t_kg> const& right
) {
  return MesiType<T, t_m, t_s, t_kg>(left.val - right.val);
}

template<typename T, int t_m, int t_m2, int t_s, int t_s2, int t_kg, int t_kg2>
constexpr MesiType<T, t_m + t_m2, t_s + t_s2, t_kg + t_kg2> operator*(
    MesiType<T, t_m, t_s, t_kg> const& left,
    MesiType<T, t_m2, t_s2, t_kg2> const& right
) {
  return MesiType<T, t_m + t_m2, t_s + t_s2, t_kg + t_kg2>(left.val * right.val);
}

template<typename T, int t_m, int t_m2, int t_s, int t_s2, int t_kg, int t_kg2>
constexpr MesiType<T, t_m - t_m2, t_s - t_s2, t_kg - t_kg2> operator/(
    MesiType<T, t_m, t_s, t_kg> const& left,
    MesiType<T, t_m2, t_s2, t_kg2> const& right
) {
  return MesiType<T, t_m - t_m2, t_s - t_s2, t_kg - t_kg2>(left.val / right.val);
}

/*
 * Unary operators, to help with literals (and general usage!)
 */

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator-(
    MesiType<T, t_m, t_s, t_kg> const& op
) {
  return MesiType<T, t_m, t_s, t_kg>(-op.val);
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator+(
    MesiType<T, t_m, t_s, t_kg> const& op
) {
  return op;
}

/*
 * Scalers by non-SI values (allows things like 2 * 3._m
 */

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator*(
    MesiType<T, t_m, t_s, t_kg> const& left,
    T const& right
) {
  return MesiType<T, t_m, t_s, t_kg>(left.val * right);
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator*(
    T const & left,
    MesiType<T, t_m, t_s, t_kg> const& right
) {
  return right * left;
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, t_m, t_s, t_kg> operator/(
    MesiType<T, t_m, t_s, t_kg> const& left,
    T const& right
) {
  return MesiType<T, t_m, t_s, t_kg>(left.val / right);
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr MesiType<T, -t_m, -t_s, -t_kg> operator/(
    T const & left,
    MesiType<T, t_m, t_s, t_kg> const& right
) {
  return MesiType<T, -t_m, -t_s, -t_kg>( left / right.val );
}

/*
 * Comparison operators
 */
template<typename T, int t_m, int t_s, int t_kg>
constexpr bool operator==(
  MesiType<T, t_m, t_s, t_kg> const& left,
  MesiType<T, t_m, t_s, t_kg> const& right
) {
  return left.val == right.val;
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr bool operator<(
  MesiType<T, t_m, t_s, t_kg> const& left,
  MesiType<T, t_m, t_s, t_kg> const& right
) {
  return left.val < right.val;
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr bool operator!=(
  MesiType<T, t_m, t_s, t_kg> const& left,
  MesiType<T, t_m, t_s, t_kg> const& right
) {
  return !(right == left);
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr bool operator<=(
  MesiType<T, t_m, t_s, t_kg> const& left,
  MesiType<T, t_m, t_s, t_kg> const& right
) {
  return left < right || left == right;
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr bool operator>(
  MesiType<T, t_m, t_s, t_kg> const& left,
  MesiType<T, t_m, t_s, t_kg> const& right
) {
  return right < left;
}

template<typename T, int t_m, int t_s, int t_kg>
constexpr bool operator>=(
  MesiType<T, t_m, t_s, t_kg> const& left,
  MesiType<T, t_m, t_s, t_kg> const& right
) {
  return left > right || left == right;
}

/*
 * Stream operators
 */

template<typename T, int t_m, int t_s, int t_kg>
std::ostream& operator<<(std::ostream & os, MesiType<T, t_m, t_s, t_kg> mt) {
	return os << mt.val << mt.getUnit();
}

/*
 * Literal operators, to allow quick creation of basic types
 * Note
 */

#ifndef MESI_LITERAL_TYPE
#   define MESI_LITERAL_TYPE float
#endif

namespace Mesi {
  using Meters    = MesiType<MESI_LITERAL_TYPE, 1, 0, 0>;
  using MetersSq  = MesiType<MESI_LITERAL_TYPE, 2, 0, 0>;
  using Seconds   = MesiType<MESI_LITERAL_TYPE, 0, 1, 0>;
  using SecondsSq = MesiType<MESI_LITERAL_TYPE, 0, 2, 0>;
  using Kilos     = MesiType<MESI_LITERAL_TYPE, 0, 0, 1>;
  using KilosSq   = MesiType<MESI_LITERAL_TYPE, 0, 0, 2>;
  using Newtons   = MesiType<MESI_LITERAL_TYPE, 1, -2, 1>;
  using NewtonsSq = MesiType<MESI_LITERAL_TYPE, 2, -4, 2>;
	using Scalar    = MesiType<MESI_LITERAL_TYPE, 0, 0, 0>;
}

constexpr Mesi::Meters operator "" _m(long double arg) {
  return Mesi::Meters(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Seconds operator "" _s(long double arg) {
  return Mesi::Seconds(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Kilos operator "" _kg(long double arg) {
  return Mesi::Kilos(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Newtons operator "" _N(long double arg) {
  return Mesi::Newtons(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Meters operator "" _m(unsigned long long arg) {
  return Mesi::Meters(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Seconds operator "" _s(unsigned long long arg) {
  return Mesi::Seconds(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Kilos operator "" _kg(unsigned long long arg) {
  return Mesi::Kilos(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::Newtons operator "" _N(unsigned long long arg) {
  return Mesi::Newtons(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::MetersSq operator "" _m2(long double arg) {
  return Mesi::MetersSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::SecondsSq operator "" _s2(long double arg) {
  return Mesi::SecondsSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::KilosSq operator "" _kg2(long double arg) {
  return Mesi::KilosSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::NewtonsSq operator "" _N2(long double arg) {
  return Mesi::NewtonsSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::MetersSq operator "" _m2(unsigned long long arg) {
  return Mesi::MetersSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::SecondsSq operator "" _s2(unsigned long long arg) {
  return Mesi::SecondsSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::KilosSq operator "" _kg2(unsigned long long arg) {
  return Mesi::KilosSq(static_cast<MESI_LITERAL_TYPE>(arg));
}

constexpr Mesi::NewtonsSq operator "" _N2(unsigned long long arg) {
  return Mesi::NewtonsSq(static_cast<MESI_LITERAL_TYPE>(arg));
}
