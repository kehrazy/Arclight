/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 noisemix.hpp
 */

#pragma once

#include "noisebase.hpp"


namespace CC {

	namespace Detail {

		template<class T>
		struct NoiseMixable;

	}

	template<class T>
	concept NoiseMixable = Detail::NoiseMixable<T>::Value;

}


template<CC::NoiseMixable... Types>
class NoiseMix {

	template<class T, class... Pack>
	using ArgsHelper = T;

public:

	static constexpr u32 TypesCount = sizeof...(Types);

	static constexpr u32 MixesCount = []() constexpr {

		u32 count = 0;

		return ((count += CC::NoiseType<Types> ? 0 : 1), ...);

	}();

	static constexpr bool Recursive = MixesCount != 0;

	static_assert(TypesCount > 1, "Cannot mix less than 2 noise types");

	template<CC::Arithmetic C, SizeT N> requires(N == TypesCount - 1)
	using ContributionT = const C (&)[N];


	constexpr NoiseMix(const Types&... types) : types(types...) {};

	constexpr NoiseMix() = default;


	template<SizeT I> requires(I < TypesCount)
	inline void permutate(u32 seed) {
		std::get<I>(types).permutate(seed);
	}

	template<SizeT I> requires(I < TypesCount)
	inline void permutate() {
		std::get<I>(types).permutate();
	}


	template<SizeT I> requires(I < TypesCount)
	constexpr TT::NthPackType<I, Types...> get() {
		return std::get<I>(types);
	}


	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>>
	constexpr F sample(const T& point, const NoiseParams<A, L, P>& params) const {

		F sample;

		std::apply([&](const auto&... args) constexpr {
			sample = (args.sample(point, params) + ...);
		}, types);

		return sample / TypesCount;

	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>, CC::Arithmetic C, SizeT N> requires(!Recursive)
	constexpr F sample(const T& point, const NoiseParams<A, L, P>& params, ContributionT<C, N> contribution) const {

		F sample = 0;

		auto calculate = [&](const auto& type, u32 idx) constexpr {

			F scale = (idx == 0) ? 1 : contribution[idx - 1];

			sample += type.sample(point, params) * scale;

			if (idx != TypesCount - 1) {
				sample *= (1 - contribution[idx]);
			}

		};

		std::apply([&](const auto&... args) constexpr {
			u32 idx = 0;
			(calculate(args, idx++), ...);
		}, types);

		return sample;

	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>, CC::Returns<F, ArgsHelper<F, Types>...> Func> requires(!Recursive)
	constexpr F sample(const T& point, const NoiseParams<A, L, P>& params, Func&& transform) const {

		F sample;

		std::apply([&](const auto&... args) constexpr {
			sample = transform(args.sample(point, params)...);
		}, types);

		return sample;

	}


	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> sample(std::span<const T> points, const NoiseParams<A, L, P>& params) const {

		const u32 count = points.size();

		std::vector<F> out(count);

		auto calculate = [&](const auto& type) constexpr {

			std::vector<F> samples = type.sample(points, params);

			for (u32 i = 0; i < count; i++) {
				out[i] += samples[i];
			}

		};

		std::apply([&](const auto&... args) constexpr {
			(calculate(args), ...);
		}, types);

		for (F& sample : out) {
			sample /= TypesCount;
		}

		return out;

	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>, CC::Arithmetic C, SizeT N> requires(!Recursive)
	constexpr std::vector<F> sample(std::span<const T> points, const NoiseParams<A, L, P>& params, ContributionT<C, N> contribution) const {

		const u32 count = points.size();

		std::vector<F> out(count);

		auto calculate = [&](const auto& type, u32 idx) constexpr {

			F scale = (idx == 0) ? 1 : contribution[idx - 1];

			std::vector<F> samples = type.sample(points, params);

			for (u32 i = 0; i < count; i++) {

				out[i] += samples[i] * scale;

				if (idx != TypesCount - 1) {
					out[i] *= 1 - contribution[idx];
				}
			}

		};

		std::apply([&](const auto&... args) constexpr {
			u32 idx = 0;
			(calculate(args, idx++), ...);
		}, types);

		return out;

	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Float F = TT::CommonArithmeticType<T>, CC::Returns<F, ArgsHelper<F, Types>...> Func> requires(!Recursive)
	constexpr std::vector<F> sample(std::span<const T> points, const NoiseParams<A, L, P>& params, Func&& transform) const {

		const u32 count = points.size();

		std::vector<F> out(count);

		auto calculate = [&](const auto&... samples) constexpr {

			for (u32 i = 0; i < count; i++) {
				out[i] = transform(samples[i]...);
			}

		};

		std::apply([&](const auto&... args) constexpr {
			calculate(args.sample(points, params)...);
		}, types);

		return out;

	}

private:

	std::tuple<Types...> types;

};


namespace CC::Detail {

	template<class T>
	struct NoiseMixable {
		constexpr static bool Value = false;
	};

	template<NoiseType T>
	struct NoiseMixable<T> {
		constexpr static bool Value = true;
	};

	template<class... Types>
	struct NoiseMixable<NoiseMix<Types...>> {
		constexpr static bool Value = true;
	};

}
