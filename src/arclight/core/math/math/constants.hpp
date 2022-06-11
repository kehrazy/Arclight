/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 constants.hpp
 */

#pragma once

#include "common/concepts.hpp"



namespace MathX {

	using MathConstantType = double;

	namespace Constants {

		template<CC::Float F> constexpr inline F pi         = 3.1415926535897932384626433832795028841971693993751058209749445923L;
		template<CC::Float F> constexpr inline F e          = 2.7182818284590452353602874713526624977572470936999595749669676277L;
		template<CC::Float F> constexpr inline F sqrt2      = 1.4142135623730950488016887242096980785696718753769480731766797380L;
		template<CC::Float F> constexpr inline F sqrt3      = 1.7320508075688772935274463415058723669428052538103806280558069795L;
		template<CC::Float F> constexpr inline F invSqrt2   = 0.7071067811865475244008443621048490392848359376884740365883398690L;
		template<CC::Float F> constexpr inline F invSqrt3   = 0.5773502691896257645091487805019574556476017512701268760186023265L;
		template<CC::Float F> constexpr inline F invPi      = 0.3183098861837906715377675267450287240689192914809128974953346881L;
		template<CC::Float F> constexpr inline F invSqrtPi  = 0.5641895835477562869480794515607725858440506293289988568440857217L;
		template<CC::Float F> constexpr inline F ln2        = 0.6931471805599453094172321214581765680755001343602552541206800095L;
		template<CC::Float F> constexpr inline F ln10       = 2.3025850929940456840179914546843642076011014886287729760333279010L;
		template<CC::Float F> constexpr inline F gamma      = 0.5772156649015328606065120900824024310421593359399235988057672349L;
		template<CC::Float F> constexpr inline F phi        = 1.6180339887498948482045868343656381177203091798057628621354486227L;

	}

	constexpr inline MathConstantType pi        = Constants::pi       <MathConstantType>;
	constexpr inline MathConstantType e         = Constants::e        <MathConstantType>;
	constexpr inline MathConstantType sqrt2     = Constants::sqrt2    <MathConstantType>;
	constexpr inline MathConstantType sqrt3     = Constants::sqrt3    <MathConstantType>;
	constexpr inline MathConstantType invSqrt2  = Constants::invSqrt2 <MathConstantType>;
	constexpr inline MathConstantType invSqrt3  = Constants::invSqrt3 <MathConstantType>;
	constexpr inline MathConstantType invPi     = Constants::invPi    <MathConstantType>;
	constexpr inline MathConstantType invSqrtPi = Constants::invSqrtPi<MathConstantType>;
	constexpr inline MathConstantType ln2       = Constants::ln2      <MathConstantType>;
	constexpr inline MathConstantType ln10      = Constants::ln10     <MathConstantType>;
	constexpr inline MathConstantType gamma     = Constants::gamma    <MathConstantType>;
	constexpr inline MathConstantType phi       = Constants::phi      <MathConstantType>;

}