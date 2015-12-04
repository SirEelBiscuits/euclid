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
		return y & 1? this->y : x;
	}

	BaseType const &operator()(unsigned, unsigned y) const {
		return y & 1? this->y : x;
	}

	BaseType &operator[](unsigned y) {
		return y & 1? this->y : x;
	}

	BaseType const &operator[](unsigned y) const {
		return y & 1? this->y : x;
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
		return BaseType(std::sqrt(DownCastType(LengthSquared())));
	}

	static constexpr Matrix Zero() {
		return Matrix();
	}
};

template<typename BaseType, typename DownCastType>
class Matrix<BaseType, 1, 3, DownCastType> {
public:
	BaseType x,y,z;

	Matrix() : x(0), y (0), z(0) {}
	Matrix(BaseType x, BaseType y, BaseType z)
		: x(x)
		, y(y)
		, z(z)
	{}

	Matrix& operator=(Matrix const &other) = default;
	BaseType &operator()(unsigned, unsigned y) {
		switch(y) {
		case 0: return x;
		case 1: return this->y;
		case 2: return z;
		default: ASSERT(false);
		}
		return x;
	}

	BaseType const &operator()(unsigned, unsigned y) const {
		switch(y){
		case 0: return x;
		case 1: return this->y;
		case 2: return z;
		default: ASSERT(false);
		}
		return x;
	}

	BaseType &operator[](unsigned y) {
		switch(y){
		case 0: return x;
		case 1: return this->y;
		case 2: return z;
		default: ASSERT(false);
		}
		return x;
	}

	BaseType const &operator[](unsigned y) const {
		switch(y){
		case 0: return x;
		case 1: return this->y;
		case 2: return z;
		default: ASSERT(false);
		}
		return x;
	}

	Matrix &operator+=(Matrix const &other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Matrix &operator-=(Matrix const &other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	template<typename ScalarType>
	Matrix &operator*=(ScalarType const &other) {
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	template<typename ScalarType>
	Matrix &operator/=(ScalarType const &other) {
		x /= other;
		y /= other;
		z /= other;
		return *this;
	}

	auto LengthSquared() {
		return x*x + y*y + z*z;
	}

	BaseType Length() {
		return BaseType(std::sqrt(DownCastType(LengthSquared())));
	}

	static constexpr Matrix Zero() {
		return Matrix(0, 0, 0);
	}
};

template<typename BaseType, typename DownCastType>
class Matrix<BaseType, 2, 2, DownCastType> {
public:
	BaseType data[2][2];

	Matrix() : data{{0, 0}, {0, 0}} {}
	Matrix(BaseType a, BaseType b, BaseType c, BaseType d)
		: data {
			{a, b},
			{c, d}
		}
	{}

	Matrix& operator=(Matrix const &other) = default;
	BaseType &operator()(unsigned x, unsigned y) {
		ASSERT(x < 2);
		ASSERT(y < 2);
		return data(x, y);
	}

	BaseType const &operator()(unsigned x, unsigned y) const {
		ASSERT(x < 2);
		ASSERT(y < 2);
		return data(x, y);
	}

	Matrix &operator+=(Matrix const &other) {
		data[0][0] += other.data[0][0];
		data[0][1] += other.data[0][1];
		data[1][0] += other.data[1][0];
		data[1][1] += other.data[1][1];
		return *this;
	}

	Matrix &operator-=(Matrix const &other) {
		data[0][0] -= other.data[0][0];
		data[0][1] -= other.data[0][1];
		data[1][0] -= other.data[1][0];
		data[1][1] -= other.data[1][1];
		return *this;
	}

	template<typename ScalarType>
	Matrix &operator*=(ScalarType const &other) {
		data[0][0] *= other;
		data[0][1] *= other;
		data[1][0] *= other;
		data[1][1] *= other;
		return *this;
	}

	template<typename ScalarType>
	Matrix &operator/=(ScalarType const &other) {
		data[0][0] /= other;
		data[0][1] /= other;
		data[1][0] /= other;
		data[1][1] /= other;
		return *this;
	}

	static constexpr Matrix Zero() {
		return Matrix(0, 0, 0, 0);
	}

	static constexpr Matrix Identity() {
		return Matrix(1, 0, 0, 1);
	}

	void Transpose() {
		std::swap(data[1][0], data[0][1]);
	}

	void Invert() {
		auto det = GetDeterminant();
		Transpose();
		operator/=(det);
	}

	auto Inverse() {
		auto det = GetDeterminant();
		auto ret = *this;
		ret.Transpose();
		ret /= det;
		return ret;
	}

	X_TO_POWER_TYPE(BaseType(0), 2) GetDeterminant() const {
		return data[0][0] * data[1][1] - data[1][0] * data[0][1];
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
> auto operator*( //should be constexpr, fucking VS
	Matrix<BaseType, T_w, T_h, DownCastType> left,
	ScalarType const &right
) {
	for(auto y = 0u; y < T_h; ++y)
		for(auto x = 0u; x < T_w; ++x)
			left(x, y) *= right;
	return left;
}

template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	typename DownCastType
> auto operator*( //should be constexpr, fucking VS
	Matrix<BaseType, 1, 2, DownCastType> left,
	ScalarType const &right
) {
	left.x *= (DownCastType)right;
	left.y *= (DownCastType)right;
	return left;
}

template<
	typename ScalarType,
	typename = std::enable_if_t<!is_matrix<ScalarType>::value, ScalarType>,
	typename BaseType,
	unsigned T_w,
	unsigned T_h,
	typename DownCastType
> auto operator*( //should be constexpr, fucking VS
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
> auto operator/( //should be constexpr, fucking VS
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
	Matrix<BaseType, T_w, T_h, DownCastType> left,
	Matrix<BaseType, T_w, T_h, DownCastType> const &right
) {
	left += right;
	return left;
}

template<typename BaseType, unsigned T_w, unsigned T_h, typename DownCastType>
constexpr Matrix<BaseType, T_w, T_h, DownCastType> operator-(
	Matrix<BaseType, T_w, T_h, DownCastType> left,
	Matrix<BaseType, T_w, T_h, DownCastType> const &right
) {
	left -= right;
	return left;
}

//vector operators

template<typename BaseType1, typename BaseType2, typename DownCastType1, typename DownCastType2>
auto operator^(
	Matrix<BaseType1, 1u, 2u, DownCastType1> const &left,
	Matrix<BaseType2, 1u, 2u, DownCastType2> const &right
) {
	return left.x * right.y - left.y * right.x;
}

template<typename BaseType1, typename BaseType2, typename DownCastType1, typename DownCastType2>
auto operator|(
	Matrix<BaseType1, 1u, 2u, DownCastType1> const &left,
	Matrix<BaseType2, 1u, 2u, DownCastType2> const &right
) {
	return left.x * right.x + left.y * right.y;
}

template<typename BaseType1, typename BaseType2, typename DownCastType1, typename DownCastType2>
auto operator|(
	Matrix<BaseType1, 1u, 3u, DownCastType1> const &left,
	Matrix<BaseType2, 1u, 3u, DownCastType2> const &right
) {
	return left.x * right.x + left.y * right.y + left.z * right.z;
}

template<typename BaseType1, typename BaseType2,  typename DowncastType1, typename DowncastType2>
auto operator*(Matrix<BaseType1, 2, 2, DowncastType1> const &left, Matrix<BaseType2, 1, 2, DowncastType2> const &right) {
	Matrix<
		std::remove_const_t<decltype(left(0,0) * right.x)>,
		1, 2,
		std::remove_const_t<decltype(DowncastType1(0) * DowncastType2(0))>
	> ret;

	ret.x = left.data[0][0] * right.x + left.data[1][0] * right.y;
	ret.y = left.data[0][1] * right.x + left.data[1][1] * right.y;
	return ret;
}
