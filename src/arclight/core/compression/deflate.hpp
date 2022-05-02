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
#include "types.hpp"

std::vector<u8> deflate(std::span<u8> data);
