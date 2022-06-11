/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 abs.hpp
 */

#pragma once

#include "fpintrinsic.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "util/bits.hpp"



namespace MathX {

	template<CC::Arithmetic T>
	constexpr T abs(T x) noexcept {

		if constexpr (CC::UnsignedIntegral<T>) {

			return x;

		} else if constexpr (CC::SignedIntegral<T>) {

			TT::MakeUnsigned<T> y = x >> (Bits::bitCount<T>() - 1);
			return (x ^ y) + (y & 1);

		} else {

			return FPIntrinsic::abs(x);

		}

	}

}