/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fpintrinsic.hpp
 */

#pragma once

#include "arcintrinsic.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "util/bits.hpp"



namespace MathX::FPIntrinsic {

	template<class T>
	concept IEEEFloat = CC::Float<T> && std::numeric_limits<T>::is_iec559;

	template<class T>
	concept IEEEMaskableFloat = IEEEFloat<T> && TT::HasSizedInteger<T>;


	enum class RoundingMode {
		Up,         //Ceil
		Down,       //Floor
		Zero,       //Trunc
		Infinity,   //Extrude
		NearestInf, //Round
	};


	enum class Category {
		Zero,
		Subnormal,
		Normal,
		Infinity,
		NaN
	};


	namespace IEEE754 {

		template<SizeT Size>
		struct SizedIEEEFloat {};

		//Binary32
		template<>
		struct SizedIEEEFloat<4> {

			using T = TT::UnsignedFromSize<4>;

			constexpr static SizeT SignShift = 31;
			constexpr static SizeT ExponentShift = 23;
			constexpr static SizeT MantissaShift = 0;

			constexpr static SizeT ExponentSize = 8;
			constexpr static SizeT MantissaSize = 23;
			constexpr static SizeT ExponentBias = 127;

			constexpr static T SignMask = Bits::createMask<T>(SignShift, 1);
			constexpr static T ExponentMask = Bits::createMask<T>(ExponentShift, ExponentSize);
			constexpr static T MantissaMask = Bits::createMask<T>(MantissaShift, MantissaSize);

		};

		//Binary64
		template<>
		struct SizedIEEEFloat<8> {

			using T = TT::UnsignedFromSize<8>;

			constexpr static SizeT SignShift = 63;
			constexpr static SizeT ExponentShift = 52;
			constexpr static SizeT MantissaShift = 0;

			constexpr static SizeT ExponentSize = 11;
			constexpr static SizeT MantissaSize = 52;
			constexpr static SizeT ExponentBias = 1023;

			constexpr static T SignMask = Bits::createMask<T>(SignShift, 1);
			constexpr static T ExponentMask = Bits::createMask<T>(ExponentShift, ExponentSize);
			constexpr static T MantissaMask = Bits::createMask<T>(MantissaShift, MantissaSize);

		};

		//Extended80
		template<>
		struct SizedIEEEFloat<10> {

			using T = u8;

			constexpr static SizeT SignShift = 7;       //79
			constexpr static SizeT ExponentShift = 0;   //64
			constexpr static SizeT MantissaShift = 0;   //0

			constexpr static SizeT ExponentSize = 15;
			constexpr static SizeT MantissaSize = 64;
			constexpr static SizeT ExponentBias = 16383;

		};

		//Binary128
		template<>
		struct SizedIEEEFloat<16> {

			using T = u8;

			constexpr static SizeT SignShift = 7;       //127
			constexpr static SizeT ExponentShift = 0;   //112
			constexpr static SizeT MantissaShift = 0;   //0

			constexpr static SizeT ExponentSize = 15;
			constexpr static SizeT MantissaSize = 112;
			constexpr static SizeT ExponentBias = 16383;

		};

		template<IEEEFloat F>
		using FloatTraits = SizedIEEEFloat<sizeof(F)>;


		namespace Constants {

			template<IEEEFloat F> constexpr inline F pInf = +std::numeric_limits<F>::infinity();
			template<IEEEFloat F> constexpr inline F nInf = -std::numeric_limits<F>::infinity();
			template<IEEEFloat F> constexpr inline F qNaN = std::numeric_limits<F>::quiet_NaN();
			template<IEEEFloat F> constexpr inline F sNaN = std::numeric_limits<F>::signaling_NaN();
			template<IEEEFloat F> constexpr inline F pZero = 0;
			template<IEEEFloat F> constexpr inline F nZero = Bits::cast<F>(IEEE754::FloatTraits<F>::SignMask);

		}

	}


	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr auto floatToInt(F f) noexcept {
		return Bits::cast<typename Traits::T>(f);
	}


	/* Categorization */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr bool isSpecialValueOrZero(F f) noexcept {
		return ((floatToInt(f) & ~Traits::SignMask) - 1) >= (Traits::ExponentMask - 1);
	}

	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr bool isSpecialValue(F f) noexcept {
		return (floatToInt(f) & ~Traits::SignMask) >= Traits::ExponentMask;
	}

	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr bool isNormal(F f) noexcept {
		return ((floatToInt(f) - (typename Traits::T(1) << Traits::ExponentShift)) & ~Traits::SignMask) < (Traits::ExponentMask - (typename Traits::T(1) << Traits::ExponentShift));
	}

	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr bool isZero(F f) noexcept {
		return (floatToInt(f) & ~Traits::SignMask) == 0;
	}

	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr bool isInfinity(F f) noexcept {
		return (floatToInt(f) & ~Traits::SignMask) == Traits::ExponentMask;
	}

	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr bool isNaN(F f) noexcept {
		return (floatToInt(f) & ~Traits::SignMask) > Traits::ExponentMask;
	}


	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F makeSignaling(F f) noexcept {
		return Bits::cast<F>(floatToInt(f) & ~(typename Traits::T(1) << (Traits::MantissaSize - 1)));
	}

	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F makeQuiet(F f) noexcept {
		return Bits::cast<F>(floatToInt(f) | (typename Traits::T(1) << (Traits::MantissaSize - 1)));
	}


	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F copySign(F from, F to) noexcept {
		return Bits::cast<F>((floatToInt(from) & Traits::SignMask) | (floatToInt(to) & ~Traits::SignMask));
	}


	/* Standard floating point functions */
	/*
	 *  Returns the absolute value of x
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F abs(F x) noexcept {

		using T = typename Traits::T;

		T y = Bits::cast<T>(x);
		y &= ~Traits::SignMask;

		return Bits::cast<F>(y);

	}


	/*
	 *  Returns the remainder of the operation x / y
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F mod(F x, F y) noexcept {

		using T = typename Traits::T;
		constexpr SizeT DiscardingZeroes = Bits::bitCount<F>() - Traits::ExponentShift;


		//Obtain integer representations
		T ix = floatToInt(x);
		T iy = floatToInt(y);

		//Absolute values
		T ax = ix & ~Traits::SignMask;
		T ay = iy & ~Traits::SignMask;

		//Sign of x
		T sx = ix & Traits::SignMask;


		//First, deal with special values
		if (isSpecialValueOrZero(x) || isSpecialValueOrZero(y)) {

			//If any operand is nan, the result will be nan as well
			if (isNaN(x) || isNaN(y)) {
				return x + y;   //Propagate the existing nan
			}

			//If x is inf or y is 0, return nan
			if (isInfinity(x) || y == 0) {
				return Bits::cast<F>(Bits::cast<T>(IEEE754::Constants::qNaN<F>) | sx);
			}

			//If x is 0 or y is inf, return x
			//This case is trapped in this branch so no extra check is necessary
			return x;

		}

		//To start the division process, exclude the case where y >= x
		if (ay >= ax) {

			//Case y > x: Return x
			//Case y == x: Return 0 with the sign of x
			if (ay > ax) {
				return x;
			} else {
				return Bits::cast<F>(sx);
			}

		}

		//The last case to check for is subnormals
		//First, get the exponent
		i32 ex = ax >> Traits::ExponentShift;
		i32 ey = ay >> Traits::ExponentShift;

		//Then get the mantissa
		T mx = ax & Traits::MantissaMask;
		T my = ay & Traits::MantissaMask;

		//If the exponent is zero it must be a subnormal (zero is covered above)
		if (!ex) {

			//Shift to normalize the number, then adjust the exponent
			i32 z = Bits::clz(ax) - DiscardingZeroes;
			mx <<= z + 1;
			ex -= z;

		} else {

			//Set implicit mantissa bit
			mx |= T(1) << Traits::ExponentShift;

		}

		//Apply the same logic to y
		if (!ey) {

			i32 z = Bits::clz(ay) - DiscardingZeroes;
			my <<= z + 1;
			ey -= z;

		} else {

			my |= T(1) << Traits::ExponentShift;

		}


		//The exponent difference: We can shift the dividend's mantissa by this amount
		i32 expDelta = ex - ey;

		//If the divisor's mantissa is greater than the dividend's, we have to shift once more
		i32 reshift = my > mx ? 1 : 0;

		//As long as we didn't exhaust our exponent delta
		while (reshift <= expDelta) {

			expDelta -= reshift;    //Reduce delta
			mx <<= reshift;         //Shift so that mx > my
			mx -= my;               //Subtract (binary division step)

			//If mx is prematurely 0, the result is +0/-0
			if (mx == 0) {
				return Bits::cast<F>(sx);
			}

			reshift = Bits::clz(mx) - DiscardingZeroes + 1;     //Calculate the new shift
			reshift += (mx << reshift) < my;                    //Add one in case our new mx is still smaller than my

		}


		//Apply the remaining delta if it isn't exhausted yet
		mx <<= expDelta;

		//If our divisor's exponent has been 0 or less it must have been subnormal
		if (ey <= 0) {

			//Shift right to compensate the (potentially) negative exponent
			mx >>= -ey + 1;
			return Bits::cast<F>(sx | (mx & Traits::MantissaMask));

		} else {

			//The number is normal, calculate the renormalization shift
			//Note that mx is always less than a normal number since we left the loop with a subtraction (so mx will always fit the mantissa)
			i32 renormalizeShift = Bits::clz(mx) - DiscardingZeroes + 1;
			i32 er = 0;

			//We may not shift more than ey allows us
			if (renormalizeShift >= ey) {

				//Subnormal shift, one less ey since we must keep the trailing 1 in the mantissa
				//The subnormal transformation happens through er = 0
				mx <<= ey - 1;

			} else {

				//New exponent is ey adjusted by our renormalization shift
				er = ey - renormalizeShift;

				//Shift the mantissa
				mx <<= renormalizeShift;

			}

			//Reassemble the result
			T r = sx | (er << Traits::ExponentShift) | (mx & Traits::MantissaMask);
			return Bits::cast<F>(r);

		}

	}


	/*
	 *  Returns the greater value of x and y
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F max(F x, F y) noexcept {

		using T = typename Traits::T;

		if (isNaN(x)) {
			return y;
		} else if (isNaN(y)) {
			return x;
		}

		T ix = floatToInt(x);
		T iy = floatToInt(y);

		if (!((ix | iy) & ~Traits::SignMask)) {
			return ix <= iy ? x : y;
		}

		return x >= y ? x : y;

	}


	/*
	 *  Returns the smaller value of x and y
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F min(F x, F y) noexcept {

		using T = typename Traits::T;

		if (isNaN(x)) {
			return y;
		} else if (isNaN(y)) {
			return x;
		}

		T ix = floatToInt(x);
		T iy = floatToInt(y);

		if (!((ix | iy) & ~Traits::SignMask)) {
			return ix >= iy ? x : y;
		}

		return x <= y ? x : y;

	}


	/*
	 *  Truncates x, i.e. discards the fractional part
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F trunc(F x) noexcept {

		if (!std::is_constant_evaluated()) {

#ifdef ARC_TARGET_HAS_SSE4_1
			if constexpr (CC::Equal<F, float>) {

				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_round_ss(v, v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_round_sd(v, v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC));

			}
#endif

		}

		using T = typename Traits::T;

		T ix = floatToInt(x);
		T ex = ix & Traits::ExponentMask;

		constexpr T t0 = Traits::ExponentBias << Traits::ExponentShift;
		constexpr T t1 = (Traits::ExponentBias + Traits::MantissaSize) << Traits::ExponentShift;

		if (ex < t0) {

			//Below one, discard to 0
			return copySign(x, F(0));

		} else if (ex < t1) {

			//Clear all fractional mantissa bits
			T s = (t1 - ex) >> Traits::ExponentShift;
			return Bits::cast<F>(Bits::mask(ix, s));

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		//x is already an integer
		return x;

	}


	/*
	 *  Calculates the ceiling of x, i.e. the integer value that is greater than or equal to x
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F ceil(F x) noexcept {

		if (!std::is_constant_evaluated()) {

#ifdef ARC_TARGET_HAS_SSE4_1
			if constexpr (CC::Equal<F, float>) {

				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_round_ss(v, v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_round_sd(v, v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC));

			}
#endif

		}

		using T = typename Traits::T;

		T ix = floatToInt(x);
		T ex = ix & Traits::ExponentMask;

		constexpr T t0 = Traits::ExponentBias << Traits::ExponentShift;
		constexpr T t1 = (Traits::ExponentBias + Traits::MantissaSize) << Traits::ExponentShift;

		if (ex < t0) {

			//Below one, return -0 for negatives, 0 for 0 and 1 for positives
			return ix & Traits::SignMask ? IEEE754::Constants::nZero<F> : (ix == 0 ? 0 : 1);

		} else if (ex < t1) {

			//Clear all fractional mantissa bits
			T s = (t1 - ex) >> Traits::ExponentShift;
			F f = Bits::cast<F>(Bits::mask(ix, s));

			//Add one if x wasn't an integer already since positive values need to be rounded up
			if (x >= 0 && x != f) {
				f += 1;
			}

			return f;

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		//x is already an integer
		return x;

	}


	/*
	 *  Calculates the floor of x, i.e. the integer value that is less than or equal to x
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F floor(F x) noexcept {

		if (!std::is_constant_evaluated()) {

#ifdef ARC_TARGET_HAS_SSE4_1
			if constexpr (CC::Equal<F, float>) {

				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_round_ss(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_round_sd(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));

			}
#endif

		}

		using T = typename Traits::T;

		T ix = floatToInt(x);
		T ex = ix & Traits::ExponentMask;

		constexpr T t0 = Traits::ExponentBias << Traits::ExponentShift;
		constexpr T t1 = (Traits::ExponentBias + Traits::MantissaSize) << Traits::ExponentShift;

		if (ex < t0) {

			//Below one, return -1 for negatives, -0 for -0 and 0 for positives
			return ix & Traits::SignMask ? (x == 0 ? IEEE754::Constants::nZero<F> : -1) : 0;

		} else if (ex < t1) {

			//Clear all fractional mantissa bits
			T s = (t1 - ex) >> Traits::ExponentShift;
			F f = Bits::cast<F>(Bits::mask(ix, s));

			//Subtract one if x wasn't an integer already since negative values need to be rounded down
			if (x < 0 && x != f) {
				f -= 1;
			}

			return f;

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		//x is already an integer
		return x;

	}


	/*
	 *  Calculates the extrusion of x, i.e. the integer value that is x rounded towards signed infinity
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F extrude(F x) noexcept {

		if (!std::is_constant_evaluated()) {

			//If we have fast ceil/floor, take these
#ifdef ARC_TARGET_HAS_SSE4_1
			if constexpr (CC::Equal<F, float> || CC::Equal<F, double>) {
				return x >= 0 ? ceil(x) : floor(x);
			}
#endif

		}

		using T = typename Traits::T;

		T ix = floatToInt(x);
		T ex = ix & Traits::ExponentMask;

		constexpr T t0 = Traits::ExponentBias << Traits::ExponentShift;
		constexpr T t1 = (Traits::ExponentBias + Traits::MantissaSize) << Traits::ExponentShift;

		if (ex < t0) {

			//Below one, return +-O for zero, else +-1
			return ix & ~Traits::SignMask ? copySign(x, F(1)) : x;

		} else if (ex < t1) {

			//Clear all fractional mantissa bits
			T s = (t1 - ex) >> Traits::ExponentShift;
			F f = Bits::cast<F>(Bits::mask(ix, s));

			//Add/Subtract one if x wasn't an integer already
			if (x != f) {
				f += copySign(x, F(1));
			}

			return f;

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		//x is already an integer
		return x;

	}


	/*
	 *  Rounds x to the nearest integer where halfway cases are rounded away from zero, i.e. 0.5 becomes 1.0, -0.5 becomes -1.0
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F roundHalfAwayFromZero(F x) noexcept {

		using T = typename Traits::T;

		T ix = floatToInt(x);
		T ex = ix & Traits::ExponentMask;

		constexpr T t0 = (Traits::ExponentBias - 1) << Traits::ExponentShift;
		constexpr T t1 = (Traits::ExponentBias + Traits::MantissaSize) << Traits::ExponentShift;

		if (ex < t0) {

			//Below 0.5, return -0 for negatives and 0 for positives
			return Bits::cast<F>(ix & Traits::SignMask);

		} else if (ex < t1) {

			//Add/Subtract 0.5
			F y = x + copySign(x, F(0.5));
			T iy = floatToInt(y);
			T ey = iy & Traits::ExponentMask;

			//Truncate
			T s = (t1 - ey) >> Traits::ExponentShift;
			return Bits::cast<F>(Bits::mask(iy, s));

		}

		//x is already an integer
		return x;

	}


	template<RoundingMode R, IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F round(F x) noexcept {

		if constexpr (R == RoundingMode::Down) {
			return floor(x);
		} else if constexpr (R == RoundingMode::Up) {
			return ceil(x);
		} else if constexpr (R == RoundingMode::Zero) {
			return trunc(x);
		} else if constexpr (R == RoundingMode::Infinity) {
			return extrude(x);
		} else if constexpr (R == RoundingMode::NearestInf) {
			return roundHalfAwayFromZero(x);
		}

	}


	/*
	 *  Returns the next representable floating point value
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F nextFloat(F x) noexcept {

		using T = typename Traits::T;

		T ix = floatToInt(x);

		if (isNaN(x)) {
			return x;
		}

		if (ix & Traits::SignMask) {
			return ix == Traits::SignMask ? 0 : Bits::cast<F>(ix - 1);
		} else {
			return Bits::cast<F>(ix + 1);
		}

	}


	/*
	 *  Returns the previous representable floating point value
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F prevFloat(F x) noexcept {

		using T = typename Traits::T;

		T ix = floatToInt(x);

		if (isNaN(x)) {
			return x;
		}

		if (ix & Traits::SignMask) {
			return Bits::cast<F>(ix + 1);
		} else {
			return ix == 0 ? IEEE754::Constants::nZero<F> : Bits::cast<F>(ix - 1);
		}

	}


	/*
	 *  Returns the distance between two floating point values, in ULPs. If one operand is NaN, -1 is returned.
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr typename Traits::T ulpDistance(F x, F y) noexcept {

		using T = typename Traits::T;

		T ix = floatToInt(x);
		T iy = floatToInt(y);

		if (isNaN(x) || isNaN(y)) {
			return -1;
		}

		if ((ix ^ iy) & Traits::SignMask) {

			//We have different signs
			return ulpDistance(x, copySign(x, IEEE754::Constants::pZero<F>)) + ulpDistance(y, copySign(y, IEEE754::Constants::pZero<F>)) + 1;

		}

		if (ix >= iy) {
			return ix - iy;
		} else {
			return iy - ix;
		}

	}


	/*
	 *  Classifies x into one of 5 categories: Zero, Subnormal, Normal, Infinity and NaN
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr Category classify(F x) noexcept {

		using T = typename Traits::T;

		if (x == 0) {
			return Category::Zero;
		}

		T ix = floatToInt(x);
		T ex = (ix & Traits::ExponentMask) >> Traits::ExponentShift;

		if (!isSpecialValue(x)) {

			return (ix & Traits::ExponentMask) >> Traits::ExponentShift ? Category::Normal : Category::Subnormal;

		} else {

			if (isNaN(x)) {
				return Category::NaN;
			} else {
				return Category::Infinity;
			}

		}

	}


	/*
	 *  Returns the fused multiply-add operation a * b + c, rounded only once at the end
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F fma(F a, F b, F c) noexcept {

		//TODO: Check if it has FMA

		if (!std::is_constant_evaluated()) {

			if constexpr (CC::Equal<F, float>) {

				__m128 x = _mm_set_ss(a);
				__m128 y = _mm_set_ss(b);
				__m128 z = _mm_set_ss(c);
				return _mm_cvtss_f32(_mm_fmadd_ss(x, y, z));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d x = _mm_set_sd(a);
				__m128d y = _mm_set_sd(b);
				__m128d z = _mm_set_sd(c);
				return _mm_cvtsd_f64(_mm_fmadd_sd(x, y, z));

			}

		}

	}


	/*
	 *  Returns the square root of x
	 *  The result is accurate to less than 0.5 ULP. Based on the inverse square root from Moroz, Samotyy et al [2021] and Markstein's residual algorithm for square roots [2000].
	 */
	template<IEEEMaskableFloat F, class Traits = typename IEEE754::FloatTraits<F>>
	constexpr F sqrt(F x) noexcept {

		using T = typename Traits::T;
		using U = TT::MakeSigned<T>;

		static_assert(!CC::Equal<F, long double>, "sqrt for long double not supported");

		if (!std::is_constant_evaluated()) {

			if constexpr (CC::Equal<F, float>) {

#ifdef ARC_TARGET_HAS_SSE
				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_sqrt_ss(v));
#endif

			} else if constexpr (CC::Equal<F, double>) {

#ifdef ARC_TARGET_HAS_SSE2
				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_sqrt_sd(v, v));
#endif

			}

		}

		if (isNormal(x) && x > F(0)) {

			//Reduce x
			T ix = floatToInt(x);
			T ex = (ix & Traits::ExponentMask) >> Traits::ExponentShift;
			U sx = ex - Traits::ExponentBias;
			U fx = sx / 2;
			bool sb = !(ex & 1);
			T nx = Traits::ExponentBias + (sx >= 0 ? sb : -sb);

			ix &= ~Traits::ExponentMask;
			ix |= nx << Traits::ExponentShift;
			x = Bits::cast<F>(ix);

			//Inverse approximation
			F f;

			if constexpr (CC::Equal<F, float>) {

				F k1 = 2.2825186f;
				F k2 = 2.2533049f;

				ix = 0x5F1110A0 - (ix >> 1);
				f = Bits::cast<F>(ix);

				F c = x * f * f;
				f = f * (k1 - c * (k2 - c));
				c = x * f;
				c = fma(f, -c, 1.0f);
				f = fma(f, 0.5f * c, f);

			} else if constexpr (CC::Equal<F, double>) {

				F k1 = 2.28251863069107890;
				F k2 = 2.25330495841181303;
				F k3 = 1.500000000301525;

				ix = 0x5FE222142565D5C2 - (ix >> 1);
				f = Bits::cast<F>(ix);

				F c = x * f * f;
				f = f * (k1 - c * (k2 - c));
				F h = 0.5 * x;
				f = f * (k3 - h * f * f);
				c = h * f;
				c = fma(f, -c, 0.5);
				f = fma(f, c, f);

			}

			//Residual calculation
			F square = f * f;
			F error = fma(-x, square, F(1.0));
			F improved = fma(fma(error, F(0.375), F(0.5)), f * error, f);

			//Square root transform
			F sqroot = x * improved;
			F residual = fma(sqroot, -sqroot, x);
			F result = fma(residual, F(0.5) * improved, sqroot);

			//Reconstruct exponent
			T iy = floatToInt(result);
			iy += fx << Traits::ExponentShift;

			return Bits::cast<F>(iy);

		} else if (isNaN(x)) {

			//Return qNaN
			return IEEE754::Constants::qNaN<F>;

		} else if (x < F(0)) {

			//Return sNaN
			return IEEE754::Constants::sNaN<F>;

		} else if (isSpecialValueOrZero(x)) {

			//Return 0 for 0 and +inf for +inf
			return x;

		} else {

			//Subnormal case, call sqrt with transformed argument
			constexpr F Bias = Bits::cast<F>(T(Traits::ExponentBias + (Traits::ExponentBias + 1) / 2) << Traits::ExponentShift);
			constexpr F SqrtBias = Bits::cast<F>(T(Traits::ExponentBias - (Traits::ExponentBias + 1) / 4) << Traits::ExponentShift);

			return SqrtBias * sqrt(x * Bias);

		}

	}

}