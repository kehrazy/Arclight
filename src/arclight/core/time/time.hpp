/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 time.hpp
 */

#pragma once

#include "types.hpp"

#include <string>


struct TimeData {

	constexpr TimeData() : second(0), minute(0), hour(0), day(0), month(0), year(0) {};
	constexpr TimeData(u32 second, u32 minute, u32 hour, u32 day, u32 month, u32 year) :
		second(second), minute(minute), hour(hour), day(day), month(month), year(year) {};

	u32 second;
	u32 minute;
	u32 hour;
	u32 day;
	u32 month;
	u32 year;

};


namespace Time {

	enum class Unit {
		Seconds = 0,
		Milliseconds,
		Microseconds,
		Nanoseconds
	};

	//Time since UNIX epoch
	u64 getTimeSinceEpoch(Time::Unit unit = Time::Unit::Milliseconds);

	//Returns the current TimeData
	TimeData getCurrentTime();

	//Returns the suffix corresponding to unit
	const char* getUnitSuffix(Time::Unit unit);

	//Returns a value that represents the unit as a factor. Factor 1 is used for seconds.
	constexpr double getUnitFactor(Time::Unit unit) {
		// Dumb C++: math functions are not constexpr
		// return Math::pow(1000, static_cast<u32>(unit));

		double out = 1.0;
		u32 u = static_cast<u32>(unit);
		while (u--) {
			out *= 1000.0;
		}
		return out;
	}

	//Converts the value from srcUnit to destUnit
	constexpr double convert(u64 time, Time::Unit srcUnit, Time::Unit destUnit) {

		double srcFactor = getUnitFactor(srcUnit);
		double destFactor = getUnitFactor(destUnit);
		double totalFactor = destFactor / srcFactor;

		return time * totalFactor;

	}

	//Returns a timestamp string that can be used for files or logging
	std::string getTimestamp();

}