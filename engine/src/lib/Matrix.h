#include "PortabilityHelper.h"

PREAMBLE

PRE_STD_LIB

POST_STD_LIB

#include "Maths.h"

/*
 * This version of Matrix doesn't exist :)
 */
template<typename BaseType, unsigned T_w, unsigned T_h, typename DownCastType = BaseType>
class Matrix {
};

template<typename BaseType, unsigned t_h, typename DownCastType = BaseType>
using Vector = Matrix<BaseType, 1u, t_h, DownCastType>;

template<typename BaseType, typename DownCastType>
class Matrix<BaseType, 1, 2, DownCastType> {
public:
	BaseType x,y;

	Matrix() : x(0), y (0) {}
	Matrix(BaseType x, BaseType y)
		: x(x)
		, y(y)
	{}

	Matrix& operator=(Matrix const &other) = default;
	BaseType &operator()(unsigned, unsigned y) {
		return y & 1? y : x;
	}

	BaseType const &operator()(unsigned, unsigned y) const {
		return y & 1? y : x;
	}

	BaseType &operator[](unsigned y) {
		return y & 1? y : x;
	}

	BaseType const &operator[](unsigned y) const {
		return y & 1? y : x;
	}

	Matrix &operator+=(Matrix const &other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	Matrix &operator-=(Matrix const &other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template<typename ScalarType>
	Matrix &operator*=(ScalarType const &other) {
		x *= other;
		y *= other;
		return *this;
	}

	template<typename ScalarType>
	Matrix &operator/=(ScalarType const &other) {
		x /= other;
		y /= other;
		return *this;
	}

	auto LengthSquared() {
		return x*x + y*y;
	}

	BaseType Length() {
		return std::sqrt(DownCastType(LengthSquared()));
	}

	static constexpr Matrix Zero() {
		Matrix ret;
		ret.x = BaseType(0);
		ret.y = BaseType(0);
		return ret;
	}
};

//type traits

template<typename T>
class is_matrix {
public:
	static bool const value = false;
};

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
class is_matrix<Matrix<BaseType, t_w, t_h, DownCastType>> {
public:
	static bool const value = true;
};

//unary
template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
constexpr Matrix<BaseType, t_w, t_h, DownCastType> operator+(
	Matrix<BaseType, t_w, t_h, DownCastType> const &op
) {
	return op;
}

template<typename BaseType, unsigned t_w, unsigned t_h, typename DownCastType>
constexpr Matrix<BaseType, t_w, t_h, DownCastType> operator-(
	Matrix<BaseType, t_w, t_h, DownCastType> const &op
) {
	return Matrix<BaseType, t_w, t_h, DownCastType>::Zero() - op;
}

//scalar
template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	unsigned T_w,
	unsigned T_h,
	typename DownCastType
> constexpr auto operator*(
	Matrix<BaseType, T_w, T_h, DownCastType> const &left,
	ScalarType const &right
) {
	Matrix<std::remove_const_t<decltype(left(0u, 0u) * right)>, T_w, T_h, DownCastType> ret;
	for(auto y = 0u; y < T_h; ++y)
		for(auto x = 0u; x < T_w; ++x)
			ret(x, y) = left(x, y) * right;
	return ret;
}

template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	unsigned T_w,
	unsigned T_h,
	typename DownCastType
> constexpr auto operator*(
	ScalarType const &left,
	Matrix<BaseType, T_w, T_h, DownCastType> const &right
) {
	Matrix<std::remove_const_t<decltype(left * right(0u, 0u))>, T_w, T_h, DownCastType> ret;
	for(auto y = 0u; y < T_h; ++y)
		for(auto x = 0u; x < T_w; ++x)
			ret(x, y) = left * right(x, y);
	return ret;
}

template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	unsigned T_w,
	unsigned T_h,
	typename DownCastType
> constexpr auto operator/(
	Matrix<BaseType, T_w, T_h, DownCastType> const &left,
	ScalarType const &right
) {
	Matrix<std::remove_const_t<decltype(left(0u, 0u) / right)>, T_w, T_h, DownCastType> ret;
	for(auto y = 0u; y < T_h; ++y)
		for(auto x = 0u; x < T_w; ++x)
			ret(x, y) = left(x, y) / right;
	return ret;
}

//arithmatic operators

template<typename BaseType, unsigned T_w, unsigned T_h, typename DownCastType>
constexpr Matrix<BaseType, T_w, T_h, DownCastType> operator+(
	Matrix<BaseType, T_w, T_h, DownCastType> const &left,
	Matrix<BaseType, T_w, T_h, DownCastType> const &right
) {
	auto ret = left;
	ret += right;
	return ret;
}

template<typename BaseType, unsigned T_w, unsigned T_h, typename DownCastType>
constexpr Matrix<BaseType, T_w, T_h, DownCastType> operator-(
	Matrix<BaseType, T_w, T_h, DownCastType> left,
	Matrix<BaseType, T_w, T_h, DownCastType> const &right
) {
	left -= right;
	return left;
}
