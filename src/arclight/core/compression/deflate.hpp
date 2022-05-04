/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 deflate.hpp
 */

#pragma once

#include <vector>
#include <span>
#include <stdexcept>
#include "types.hpp"

namespace Compression {
	std::vector<u8> inflate(std::span<const u8> data);
}

class CompressorException : public std::runtime_error {

public:
	explicit CompressorException(const std::string& msg) : std::runtime_error(msg) {}

};

namespace Compress = Compression;