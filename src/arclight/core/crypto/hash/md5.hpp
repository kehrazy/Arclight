/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 md5.hpp
 */

#pragma once

#include "hash.hpp"
#include "common.hpp"
#include "util/bits.hpp"
#include "util/assert.hpp"

#include <span>



namespace MD5 {

	namespace __Detail {

		//rol bit counts
		constexpr u8 rolTable[16] = {
				7, 12, 17, 22,
				5, 9, 14, 20,
				4, 11, 16, 23,
				6, 10, 15, 21
		};

		//Constant table
		constexpr u32 constantTable[64] = {
				0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE,
				0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
				0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE,
				0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
				0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA,
				0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
				0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
				0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
				0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C,
				0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
				0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
				0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
				0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039,
				0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
				0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1,
				0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
		};


		constexpr void dispatchBlock(const std::span<const u8>& data, u32& a0, u32& b0, u32& c0, u32& d0) {

			arc_assert(data.size() == 64, "MD5 block must be of size 64");

			u32 m[16];

			for(u32 i = 0; i < 16; i++) {
				m[i] = Bits::little32(Bits::assemble<u32>(data.data() + i * 4));
			}

			u32 a = a0;
			u32 b = b0;
			u32 c = c0;
			u32 d = d0;

			u32 f, g;

			for(u32 i = 0; i < 64; i++) {

				switch (i >> 4) {

					case 0:
						f = (b & c) | (~b & d);
						g = i;
						break;

					case 1:
						f = (d & b) | (~d & c);
						g = (5 * i + 1) & 0xF;
						break;

					case 2:
						f = b ^ c ^ d;
						g = (3 * i + 5) & 0xF;
						break;

					case 3:
						f = c ^ (b | ~d);
						g = (7 * i) & 0xF;
						break;

					default:
						arc_force_assert("Impossible case in MD5 block dispatch");
						break;

				}

				f += a + constantTable[i] + m[g];
				a = d;
				d = c;
				c = b;
				b = b + Bits::rol(f, rolTable[(i >> 2 & 0xC) | (i & 0x3)]);

			}

			a0 += a;
			b0 += b;
			c0 += c;
			d0 += d;

		}

	}

	constexpr Hash<128> hash(const std::span<const u8>& data) {

		Crypto::MDConstruction<64> construct;
		Crypto::mdConstruct(construct, data, ByteOrder::Little);

		u32 a = 0x67452301;
		u32 b = 0xEFCDAB89;
		u32 c = 0x98BADCFE;
		u32 d = 0x10325476;

		SizeT blocks = construct.blocks;
		SizeT specialBlocks = construct.prevBlockUsed + 1;

		//Hash blocks
		if (blocks >= specialBlocks) {

			for(SizeT i = 0; i < blocks - specialBlocks; i++) {
				__Detail::dispatchBlock(data.subspan(i * 64, 64), a, b, c, d);
			}

		}

		if (construct.prevBlockUsed) {
			__Detail::dispatchBlock({construct.prevLastBlock, 64}, a, b, c, d);
		}

		__Detail::dispatchBlock({construct.lastBlock, 64}, a, b, c, d);

		return Hash<128>(Bits::little32(a), Bits::little32(b), Bits::little32(c), Bits::little32(d));

	}

}