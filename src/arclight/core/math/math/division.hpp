/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 division.hpp
 */

#pragma once

#include "fpintrinsic.hpp"
#include "common/concepts.hpp"



namespace MathX {

	template<CC::Integer I>
	struct DivisionResult {
		I quotient;
		I remainder;
	};


	template<CC::Integer I, CC::Integer J>
	constexpr DivisionResult<I> divmod(I dividend, J divisor) noexcept {
		return { dividend / divisor, dividend % divisor };
	}


	template<CC::Arithmetic T, CC::Arithmetic U, CC::Arithmetic V = TT::CommonType<T, U>>
	constexpr V mod(T dividend, U divisor) noexcept {

		if constexpr (CC::Integral<V>) {
			return dividend % divisor;
		} else {
			return FPIntrinsic::mod(static_cast<V>(dividend), static_cast<V>(divisor));
		}

	}

}