/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 math.hpp
 */

#pragma once

#include "types.hpp"
#include "arcconfig.hpp"
#include "arcintrinsic.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"

#include <cmath>
#include <limits>
#include <cstdlib>



#if defined(ARC_CMATH_CONSTEXPR_FIX) && ARC_CMATH_CONSTEXPR_FIX
#define ARC_CMATH_CONSTEXPR constexpr
#else
#define ARC_CMATH_CONSTEXPR ARC_FORCE_INLINE
#endif



namespace Math {

	constexpr double pi = 3.1415926535897932384626434;
	constexpr double e = 2.7182818284590452353602875;
	constexpr double epsilon = 0.000001;
	constexpr double minEpsilon = 0.00000001;
	
	template<Float F> constexpr F nan_v = std::numeric_limits<F>::quiet_NaN();
	template<Float F> constexpr F inf_v = std::numeric_limits<F>::infinity();

	constexpr double nan = nan_v<double>;
	constexpr double inf = inf_v<double>;

	template<Arithmetic A>
	using PromotedType = TT::Conditional<Equal<A, long double>, A, double>;


	template<Arithmetic A> constexpr auto ceil(A);
	template<Arithmetic A> constexpr auto floor(A);
	template<Arithmetic A> constexpr auto trunc(A);
	template<Arithmetic A> constexpr auto round(A);
	template<Float F> constexpr bool isNaN(F);
	template<Float F> constexpr bool isInfinity(F);


	template<Arithmetic A, Float F = TT::ToSizedFloat<A>>
	constexpr F toDegrees(A radians) noexcept {
		return radians * F(180) / pi;
	}

	template<Arithmetic A, Float F = TT::ToSizedFloat<A>>
	constexpr F toRadians(A degrees) noexcept {
		return degrees * pi / F(180);
	}

	template<Arithmetic A>
	constexpr auto abs(A value) {

		if (std::is_constant_evaluated()) {

			if constexpr (!Float<A>) {
			
				return value < A(0) ? -value : value;

			} else {

				if (value == A(0)) {
					return A(0);
				} else if (Math::isInfinity(value)) {
					return inf;
				} else if (Math::isNaN(value)) {
					return nan;
				} else {
					return value < A(0) ? -value : value;
				}

			}

		}

		return std::abs(value);

	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic... Args>
	constexpr auto max(A a, B b, C c, Args... args) {
		return std::max({ a, b, c, args... });
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto max(A a, B b) {
		return std::max<std::common_type_t<A, B>>(a, b);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic... Args>
	constexpr auto min(A a, B b, C c, Args... args) {
		return std::min({ a, b, c, args... });
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto min(A a, B b) {
		return std::min<std::common_type_t<A, B>>(a, b);
	}

	template<class T>
	constexpr auto ascOrder(T& a, T& b) {

		if (b < a) {
			std::swap(a, b);
		}

	}

	template<class T>
	constexpr auto descOrder(T& a, T& b) {

		if (a < b) {
			std::swap(a, b);
		}

	}

	template<Integral I>
	constexpr bool isZero(I value) {
		return value == I(0);
	}

	template<Float F>
	constexpr bool isZero(F value) {
		return Math::abs(value) < minEpsilon;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool equal(A a, B b) requires (Float<A> || Float<B>) {

		auto x = Math::abs(a - b);
		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return x <= (Math::max(Math::max(y, z) * epsilon, minEpsilon));

	}

	template<Integral I, Integral J>
	constexpr bool equal(I a, J b) {
		return a == b;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool less(A a, B b) requires (Float<A> || Float<B>) {

		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return (b - a) > (Math::max(y, z) * epsilon);

	}

	template<Integral I, Integral J>
	constexpr bool less(I a, J b) {
		return a < b;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool greater(A a, B b) requires (Float<A> || Float<B>) {

		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return (a - b) > (Math::max(y, z) * epsilon);

	}

	template<Integral I, Integral J>
	constexpr bool greater(I a, J b) {
		return a > b;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool lessEqual(A a, B b) requires (Float<A> || Float<B>) {

		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return (b - a) > -Math::max(Math::max(y, z) * epsilon, minEpsilon);

	}

	template<Integral I, Integral J>
	constexpr bool lessEqual(I a, J b) {
		return a <= b;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool greaterEqual(A a, B b) requires (Float<A> || Float<B>) {

		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return (a - b) > -Math::max(Math::max(y, z) * epsilon, minEpsilon);

	}

	template<Integral I, Integral J>
	constexpr bool greaterEqual(I a, J b) {
		return a >= b;
	}

	template<Arithmetic A>
	constexpr auto sign(A value) noexcept {
		return (value > A(0)) - (value < A(0));
	}

	template<Arithmetic A>
	inline auto signbit(A value) noexcept {
		return std::signbit(value);
	}

	template<Arithmetic A>
	inline auto copysign(A value, A sgn) noexcept {
		return std::copysign(value, sgn);
	}

	template<Integral I> constexpr bool isInfinity(I) 			{ return false; }
	template<Integral I> constexpr bool isPositiveInfinity(I) 	{ return false; }
	template<Integral I> constexpr bool isNegativeInfinity(I) 	{ return false; }
	template<Integral I> constexpr bool isNaN(I) 				{ return false; }

	template<Float F>
	constexpr bool isInfinity(F value) {

		if (std::is_constant_evaluated()) {
			return value == std::numeric_limits<F>::infinity() || value == -std::numeric_limits<F>::infinity();
		}

		return std::isinf(value);

	}

	template<Float F>
	constexpr bool isPositiveInfinity(F value) {

		if (std::is_constant_evaluated()) {
			return value == std::numeric_limits<F>::infinity();
		}

		return std::isinf(value) && sign(value) == 1;

	}

	template<Float F>
	constexpr bool isNegativeInfinity(F value) {

		if (std::is_constant_evaluated()) {
			return value == -std::numeric_limits<F>::infinity();
		}

		return std::isinf(value) && sign(value) == -1;

	}

	template<Float F>
	constexpr bool isNaN(F value) {

		if (std::is_constant_evaluated()) {
			return value != value;
		}

		return std::isnan(value);

	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto sin(A radians) {
/*
		if (std::is_constant_evaluated()) {

			using F = PromotedType<A>;
			F v = F(radians);

			if (v == 0) {
				return radians;
			} else if (isInfinity(v) || isNaN(v)) {
				return nan;
			} else {
				return v * (1 - v * v * (1 / F(6) - v * v * (1 / F(120) - v * v * (1 / F(720 * 7) - v * v * (1 / F(720 * 7 * 8 * 9) - v * v / F(720 * 7 * 8 * 9 * 10 * 11))))));
				//return v - v * v * v / F(6) + v * v * v * v * v / F(120) - v * v * v * v * v * v * v / F(720 * 7) + v * v * v * v * v * v * v * v * v / F(720 * 7 * 8 * 9) - v * v * v * v * v * v * v * v * v * v * v / F(720 * 7 * 8 * 9 * 10 * 11);
			}

		}
*/
		return std::sin(radians);

	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto cos(A radians) {
		return std::cos(radians);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto tan(A radians) {
		return std::tan(radians);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto cot(A radians) {

		auto soc = tan(radians);

		if (isZero(soc))
			return copysign(radians, HUGE_VAL);

		return A(1) / soc;

	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto asin(A value) {
		return std::asin(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto acos(A value) {
		return std::acos(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto atan(A value) {
		return std::atan(value);
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto atan2(A y, B x) {
		return std::atan2(y, x);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto mod(A a, B b) requires (Float<A> || Float<B>) {

		if (std::is_constant_evaluated()) {
			return a - trunc(a / b) * b;
		}

		return std::fmod(a, b);

	}

	template<Integer A, Integer B>
	constexpr auto mod(A a, B b) {
		return a % b;
	}

	template<Float F>
	ARC_CMATH_CONSTEXPR F fract(F v) {
		[[maybe_unused]] F f;
		return std::modf(v, &f);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto exp(A exponent) {
		return A(std::exp(exponent));
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto pow(A base, B exponent) {

		if (std::is_constant_evaluated()) {

			//PromotedType<decltype(base * exponent)> x;

		}

		return std::pow(base, exponent);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto ln(A value) {
		return std::log(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto log(A value) {
		return std::log10(value);
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto log(A base, B value) {
		return Math::log(value) / Math::log(base);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto sqrt(A value) {
		return std::sqrt(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto cbrt(A value) {
		return std::cbrt(value);
	}

	template<Arithmetic A>
	constexpr auto ceil(A value) {
		return std::ceil(value);
	}

	template<Arithmetic A>
	constexpr auto floor(A value) {
		return std::floor(value);
	}

	template<Arithmetic A>
	constexpr auto trunc(A value) {
		return std::trunc(value);
	}

	template<Arithmetic A>
	constexpr auto round(A value) {
		return std::round(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto round(A value, u32 digits) noexcept {
		return (value * Math::pow(10, digits) + 0.5) / Math::pow(10, digits);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D, Arithmetic E>
	constexpr auto map(A value, B start1, C end1, D start2, E end2) noexcept {

		using F = TT::Conditional<TT::IsAnyOf<double, A, B, C, D, E>, double, float>;

		auto e = F(end2) - F(start2);
		auto s = F(end1) - F(start1);
		auto n = F(value) - F(start1);

		return start2 + e * (n / s);

	};

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr auto lerp(A start, B end, C factor) noexcept {
		return start + factor * (end - start);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr auto clamp(A value, B lowerBound, C upperBound) noexcept {
		return less(value, lowerBound) ? lowerBound : (greater(value, upperBound) ? upperBound : value);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr bool inRange(A value, B lowerBound, C upperBound) {
		return greaterEqual(value, lowerBound) && lessEqual(value, upperBound);
	}

	template<Integer I>
	constexpr bool isAligned(I x, AlignT alignment) noexcept {
		return !(x & (alignment - 1));
	}

	template<Integer I>
	constexpr I alignUp(I x, AlignT alignment) noexcept {
		return (x + alignment - 1) & ~(alignment - 1);
	}

	template<Integer I>
	constexpr I alignDown(I x, AlignT alignment) noexcept {
		return x & ~(alignment - 1);
	}

	template<Float F>
	constexpr F triangle(F x) noexcept {
		return abs(mod(x - 1, 2.0f) - 1);
	}


	enum class ConstantType {
		Zero
	};

	template<ConstantType Type>
	class ArithmeticConstant
	{
	public:

		//Zero
		template<Arithmetic A>
		constexpr auto operator<=>(A rhs) requires(Type == ConstantType::Zero) {
			return Math::isZero(rhs) ? 0 : (Math::greater(0, rhs) ? 1 : -1);
		}

		template<Arithmetic A>
		constexpr bool operator==(A rhs) requires(Type == ConstantType::Zero) {
			return Math::isZero(rhs);
		}

		template<ConstantType T>
		consteval bool operator==(ArithmeticConstant<T> t) requires(Type == ConstantType::Zero) {
			return T == ConstantType::Zero;
		}

	};

}

inline Math::ArithmeticConstant<Math::ConstantType::Zero> Zero;