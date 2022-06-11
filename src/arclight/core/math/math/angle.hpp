/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 angle.hpp
 */

#pragma once

#include "constants.hpp"
#include "common/typetraits.hpp"



namespace MathX {

	template<CC::Arithmetic T, CC::Float F = TT::ToFloat<T>>
	constexpr F toDegrees(T radians) noexcept {
		return radians / Constants::pi<F> * 180;
	}

	template<CC::Arithmetic T, CC::Float F = TT::ToFloat<T>>
	constexpr F toRadians(T degrees) noexcept {
		return degrees * Constants::pi<F> / 180;
	}

}



constexpr long double operator""_deg(long double degrees) noexcept {
    return MathX::toRadians(degrees);
}

constexpr long double operator""_deg(unsigned long long degrees) noexcept {
    return MathX::toRadians(degrees);
}