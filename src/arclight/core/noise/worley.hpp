/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 worley.hpp
 */

#pragma once

#include "noisebase.hpp"


enum class WorleyNoiseFlag {
	None,
	Second,
	Diff,
};


template<NoiseFractal Fractal, WorleyNoiseFlag Flag>
class WorleyNoiseBase : public NoiseBase {

public:

	using FlagT = WorleyNoiseFlag;


	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P>
	constexpr TT::CommonArithmeticType<T> sample(const T& point, const NoiseParams<A, L, P>& params) const {
		return fractalSample<Fractal>([this](const T& p, A f) constexpr { return raw(p, f); }, point, params);
	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> sample(std::span<const T> points, const NoiseParams<A, L, P>& params) const {
		return fractalSample<Fractal>([this](auto p, A f) constexpr { return raw(p, f); }, points, params);
	}

private:

	template<CC::Float F, CC::Arithmetic A>
	constexpr F raw(F point, A frequency) const {

		using I = TT::ToInteger<F>;

		constexpr F max = 2;

		point *= frequency;

		I ip = Math::floor(point);

		F p = point - ip;

		F first = max;
		F second = max;

		for (I ofs = -1; ofs <= 1; ofs++) {

			u32 h = ip + ofs & hashMask;

			F g = gradient<F>[hash(h) & grad1DMask];

			g = g / 2 + 0.5 + ofs;

			F dist = Math::abs(p - g);

			updateDistances(first, second, dist);
		}

		F sample = applyFlag(first, second) / max * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<CC::FloatVector V, CC::Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type raw(V point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;

		constexpr F max = 1.41421356237; // sqrt(2)

		point *= frequency;

		F x = point.x;
		F y = point.y;

		I ipx = Math::floor(x);
		I ipy = Math::floor(y);

		F px = x - ipx;
		F py = y - ipy;

		F first = max;
		F second = max;

		for (I ofsx = -1; ofsx <= 1; ofsx++) {
			for (I ofsy = -1; ofsy <= 1; ofsy++) {

				u32 hx = ipx + ofsx & hashMask;
				u32 hy = ipy + ofsy & hashMask;

				auto [gx, gy] = gradient<V>[hash(hx, hy) & grad2DMask];

				gx = gx / 2 + 0.5 + ofsx;
				gy = gy / 2 + 0.5 + ofsy;

				F dist = V(px, py).distance(V(gx, gy));

				updateDistances(first, second, dist);
			}
		}

		F sample = applyFlag(first, second) / max * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<CC::FloatVector V, CC::Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type raw(V point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;

		constexpr F max = 1.73205080756; // sqrt(3)

		point *= frequency;

		F x = point.x;
		F y = point.y;
		F z = point.z;

		I ipx = Math::floor(x);
		I ipy = Math::floor(y);
		I ipz = Math::floor(z);

		F px = x - ipx;
		F py = y - ipy;
		F pz = z - ipz;

		F first = max;
		F second = max;

		for (I ofsx = -1; ofsx <= 1; ofsx++) {
			for (I ofsy = -1; ofsy <= 1; ofsy++) {
				for (I ofsz = -1; ofsz <= 1; ofsz++) {

					u32 hx = ipx + ofsx & hashMask;
					u32 hy = ipy + ofsy & hashMask;
					u32 hz = ipz + ofsz & hashMask;

					auto [gx, gy, gz] = gradient<V>[hash(hx, hy, hz) & grad3DMask];

					gx = gx / 2 + 0.5 + ofsx;
					gy = gy / 2 + 0.5 + ofsy;
					gz = gz / 2 + 0.5 + ofsz;

					F dist = V(px, py, pz).distance(V(gx, gy, gz));

					updateDistances(first, second, dist);
				}
			}
		}

		F sample = applyFlag(first, second) / max * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<CC::FloatVector V, CC::Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type raw(V point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;

		constexpr F max = 2; // sqrt(4)

		point *= frequency;

		F x = point.x;
		F y = point.y;
		F z = point.z;
		F w = point.w;

		I ipx = Math::floor(x);
		I ipy = Math::floor(y);
		I ipz = Math::floor(z);
		I ipw = Math::floor(w);

		F px = x - ipx;
		F py = y - ipy;
		F pz = z - ipz;
		F pw = w - ipw;

		F first = max;
		F second = max;

		for (I ofsx = -1; ofsx <= 1; ofsx++) {
			for (I ofsy = -1; ofsy <= 1; ofsy++) {
				for (I ofsz = -1; ofsz <= 1; ofsz++) {
					for (I ofsw = -1; ofsw <= 1; ofsw++) {

						u32 hx = ipx + ofsx & hashMask;
						u32 hy = ipy + ofsy & hashMask;
						u32 hz = ipz + ofsz & hashMask;
						u32 hw = ipw + ofsw & hashMask;

						auto [gx, gy, gz, gw] = gradient<V>[hash(hx, hy, hz, hw) & grad3DMask];

						gx = gx / 2 + 0.5 + ofsx;
						gy = gy / 2 + 0.5 + ofsy;
						gz = gz / 2 + 0.5 + ofsz;
						gw = gw / 2 + 0.5 + ofsw;

						F dist = V(px, py, pz, pw).distance(V(gx, gy, gz, gw));

						updateDistances(first, second, dist);
					}
				}
			}
		}

		F sample = applyFlag(first, second) / max * 2 - 1;

		return applyFractal<Fractal>(sample);

	}


	template<CC::FloatParam T, CC::Arithmetic A, CC::Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> raw(std::span<const T> points, A frequency) const {

		std::vector<F> samples;

		for (u32 i = 0; i < points.size(); i++) {
			samples.push_back(raw(points[i], frequency));
		}

		return samples;
	}


	template<CC::Float F>
	static constexpr void updateDistances(F& first, F& second, F dist) {
		if constexpr (Flag == FlagT::None) {

			first = Math::min(first, dist);

		} else {

			second = Math::min(second, dist);

			if (Math::less(dist, first)) {
				second = first;
				first = dist;
			}

		}
	}

	template<CC::Float F>
	static constexpr F applyFlag(F first, F second) {
		if constexpr (Flag == FlagT::Second) {
			return second;
		} else if constexpr (Flag == FlagT::Diff) {
			return second - first;
		} else {
			return first;
		}
	}

};


using WorleyNoise				= WorleyNoiseBase<NoiseFractal::Standard,	WorleyNoiseFlag::None>;
using WorleyNoise2nd			= WorleyNoiseBase<NoiseFractal::Standard,	WorleyNoiseFlag::Second>;
using WorleyNoiseDiff			= WorleyNoiseBase<NoiseFractal::Standard,	WorleyNoiseFlag::Diff>;
using WorleyNoiseRidged			= WorleyNoiseBase<NoiseFractal::Ridged,		WorleyNoiseFlag::None>;
using WorleyNoiseRidged2nd		= WorleyNoiseBase<NoiseFractal::Ridged,		WorleyNoiseFlag::Second>;
using WorleyNoiseRidgedDiff		= WorleyNoiseBase<NoiseFractal::Ridged,		WorleyNoiseFlag::Diff>;
using WorleyNoiseRidgedSq		= WorleyNoiseBase<NoiseFractal::RidgedSq,	WorleyNoiseFlag::None>;
using WorleyNoiseRidgedSq2nd	= WorleyNoiseBase<NoiseFractal::RidgedSq,	WorleyNoiseFlag::Second>;
using WorleyNoiseRidgedSqDiff	= WorleyNoiseBase<NoiseFractal::RidgedSq,	WorleyNoiseFlag::Diff>;
