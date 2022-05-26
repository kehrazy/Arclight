/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 noisebase.hpp
 */

#pragma once

#include "math/math.hpp"
#include "math/vector.hpp"
#include "common/concepts.hpp"
#include <numeric>
#include <random>
#include <array>


enum class NoiseFractal {
	Standard,
	Ridged,
	RidgedSq,
};


template<CC::Arithmetic F, CC::Arithmetic L = u32, CC::Arithmetic P = u32>
struct NoiseParams {

	constexpr bool operator==(const NoiseParams&) const noexcept = default;

	F frequency = 1;
	u32 octaves = 1;
	L lacunarity = 1;
	P persistence = 1;
};


class NoiseBase {

public:

	constexpr NoiseBase() : p(defaultP) {};


	inline void permutate(u32 seed) {
		p = genPermutation(seed);
	}

	inline void permutate() {
		std::random_device rd;
		permutate(rd());
	}

protected:

	template<NoiseFractal Fractal, CC::Float F>
	static constexpr F applyFractal(F sample) {

		if constexpr (Fractal != NoiseFractal::Standard) {

			sample = 1 - Math::abs(sample);

			if constexpr (Fractal == NoiseFractal::RidgedSq) {
				sample *= sample;
			}

			sample = sample * 2 - 1;
		}

		return sample;

	}

	template<NoiseFractal Fractal, CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>>
	static constexpr F fractalSample(auto&& func, const T& point, const NoiseParams<A, L, P>& params) {

		auto [frequency, octaves, lacunarity, persistence] = params;

		arc_assert(octaves >= 1, "Octaves count cannot be 0");

		if (octaves == 1) {
			return func(point, frequency);
		}

		F scale = 1;
		F noise = 0;
		F range = 0;

		for (u32 i = 0; i < octaves; i++) {

			F sample = func(point, frequency);

			noise += sample * scale;

			range += scale;
			frequency *= lacunarity;

			if constexpr (Fractal == NoiseFractal::Standard) {
				scale *= persistence;
			} else {
				scale *= 1 - Math::abs(sample);
				scale *= 0.5;
			}
		}

		return noise / range;

	}

	template<NoiseFractal Fractal, CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>>
	static constexpr std::vector<F> fractalSample(auto&& func, std::span<const T> points, const NoiseParams<A, L, P>& params) {

		auto [frequency, octaves, lacunarity, persistence] = params;

		arc_assert(octaves >= 1, "Octaves count cannot be 0");
		arc_assert(points.size() >= 0, "Points count cannot be 0");

		if (octaves == 1) {
			return func(points, frequency);
		}

		arc_force_assert("Fractal span sampling not yet supported");

		return {};

	}


	static constexpr u32 grad1DMask = 0x1;
	static constexpr u32 grad2DMask = 0x7;
	static constexpr u32 grad3DMask = 0xF;
	static constexpr u32 grad4DMask = 0x1F;


	template<class T>
	static constexpr T gradient;

	template<CC::Float F>
	static constexpr F gradient<F>[2] = {
		-1, 1
	};

	template<CC::FloatVector V> requires(V::Size == 2)
	static constexpr V gradient<V>[8] = {
		{ 0.707107, 0.707107}, { 1, 0},
		{-0.707107, 0.707107}, {-1, 0},
		{ 0.707107,-0.707107}, { 0, 1},
		{-0.707107,-0.707107}, { 0,-1}
	};

	template<CC::FloatVector V> requires(V::Size == 3)
	static constexpr V gradient<V>[16] = {
		{ 0.57735, 0.57735,-0.57735}, { 0.707107, 0.707107, 0},
		{-0.57735, 0.57735, 0.57735}, {-0.707107, 0.707107, 0},
		{-0.57735, 0.57735,-0.57735}, { 0.707107,-0.707107, 0},
		{ 0.57735,-0.57735, 0.57735}, {-0.707107,-0.707107, 0},
		{ 0.57735,-0.57735,-0.57735}, { 1, 0, 0},
		{-0.57735,-0.57735, 0.57735}, {-1, 0, 0},
		{-0.57735,-0.57735,-0.57735}, { 0, 1, 0},
		{ 0.57735, 0.57735, 0.57735}, { 0, 0, 1}
	};

	template<CC::FloatVector V> requires(V::Size == 4)
	static constexpr V gradient<V>[32] = {
		{-0.5, 0.5,-0.5,-0.5}, {-0.57735, 0.57735,-0.57735, 0},
		{ 0.5,-0.5,-0.5,-0.5}, { 0.57735,-0.57735,-0.57735, 0},
		{-0.5,-0.5,-0.5,-0.5}, {-0.57735,-0.57735,-0.57735, 0},
		{-0.5, 0.5, 0.5,-0.5}, {-0.57735, 0.57735, 0.57735, 0},
		{ 0.5,-0.5, 0.5,-0.5}, { 0.57735,-0.57735, 0.57735, 0},
		{ 0.5, 0.5, 0.5,-0.5}, { 0.57735, 0.57735, 0.57735, 0},
		{ 0.5, 0.5,-0.5, 0.5}, { 0.57735, 0.57735, 0,-0.57735},
		{-0.5,-0.5, 0.5, 0.5}, { 0.57735,-0.57735, 0,-0.57735},
		{-0.5, 0.5,-0.5, 0.5}, {-0.57735,-0.57735, 0,-0.57735},
		{ 0.5, 0.5, 0.5, 0.5}, { 0.57735, 0.57735, 0, 0.57735},
		{ 0.5,-0.5,-0.5, 0.5}, {-0.57735, 0.57735, 0, 0.57735},
		{-0.5, 0.5, 0.5, 0.5}, {-0.57735,-0.57735, 0, 0.57735},
		{ 0.707107, 0.707107, 0, 0},
		{ 0.707107,-0.707107, 0, 0},
		{-0.707107,-0.707107, 0, 0},
		{-1, 0, 0, 1},
		{ 0, 0, 1, 1},
		{ 0, 1, 0, 0},
		{ 0, 0, 1, 0},
		{ 0, 1, 0,-1},
	};


	static constexpr u32 hashMask = 0xFF;

	constexpr u32 hash(u32 x) const {
		return p[x];
	}

	constexpr u32 hash(u32 x, u32 y) const {
		return p[hash(x) + y];
	}

	constexpr u32 hash(u32 x, u32 y, u32 z) const {
		return p[hash(x, y) + z];
	}

	constexpr u32 hash(u32 x, u32 y, u32 z, u32 w) const {
		return p[hash(x, y, z) + w];
	}

private:

	using PermutationT = std::array<u32, 512>;

	static PermutationT genPermutation(u32 seed) {

		PermutationT p;

		std::iota(p.begin(), p.begin() + 256, 0);
		std::mt19937 rng(seed);
		std::shuffle(p.begin(), p.begin() + 256, rng);
		std::copy(p.begin(), p.begin() + 256, p.begin() + 256);

		return p;

	}

	static constexpr u32 defaultSeed = 0xA6C;

	static inline const PermutationT defaultP = genPermutation(defaultSeed);

	PermutationT p;

};


namespace CC {

	template<class T>
	concept NoiseType = BaseOf<NoiseBase, T>;

}
