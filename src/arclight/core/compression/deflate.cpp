/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 deflate.cpp
 */

#include "deflate.hpp"
#include "util/log.hpp"
#include "debug.hpp"



constexpr u8 shuffle[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

std::vector<u8> Compress::deflate(std::span<const u8> data) {
	u64 pos = 0;
	bool last = false;

	while (!last) {

		last = data[pos / 8] >> (pos % 8);
		pos++;

		u8 type;
		if (pos % 8 == 7) {
			type = data[(pos)/8] >> 6 | data[(pos) / 8 + 1];
			pos += 2;
		} else {
			type = (data[pos/8] >> (pos % 8)) & 0x3;
			pos += 2;
		}

		Log::debug("DEFLATE", "Type: %i", (int) type);

		switch (type) {
		case 2: {

			u16 HLIT;
			u8 HDIST, HCLEN;

			switch (pos % 8) {
			case 0:
				HLIT = data[pos / 8] & 0x1F;
				HDIST = (data[pos / 8] >> 5) + ((data[pos / 8 + 1] & 0x3) << 3);
				HCLEN = (data[pos / 8 + 1] >> 2) & 0xF;
				break;
			case 1:
				HLIT = (data[pos / 8] >> 1) & 0x1F;
				HDIST = (data[pos / 8] >> 6) + ((data[pos / 8 + 1] & 0x7) << 2);
				HCLEN = (data[pos / 8 + 1] >> 3) & 0xF;
				break;
			case 2:
				HLIT = (data[pos / 8] >> 2) & 0x1F;
				HDIST = (data[pos / 8] >> 7) + ((data[pos / 8 + 1] & 0xF) << 1);
				HCLEN = data[pos / 8 + 1] >> 4;
				break;
			case 3:
				HLIT = data[pos / 8] >> 3;
				HDIST = data[pos / 8 + 1] & 0x1F;
				HCLEN = (data[pos / 8 + 1] >> 5) + ((data[pos / 8 + 2] & 0x1) << 3);
				break;
			case 4:
				HLIT = (data[pos / 8] >> 4) + ((data[pos / 8 + 1] & 0x1) << 4);
				HDIST = (data[pos / 8 + 1] >> 1) & 0x1F;
				HCLEN = (data[pos / 8 + 1] >> 6) + ((data[pos / 8 + 2] & 0x3) << 2);
				break;
			case 5:
				HLIT = (data[pos / 8] >> 5) + ((data[pos / 8 + 1] & 0x3) << 3);
				HDIST = (data[pos / 8 + 1] >> 2) & 0x1F;
				HCLEN = (data[pos / 8 + 1] >> 7) + ((data[pos / 8 + 2] & 0x7) << 1);
				break;
			case 6:
				HLIT = (data[pos / 8] >> 6) + ((data[pos / 8 + 1] & 0x7) << 2);
				HDIST = (data[pos / 8 + 1] >> 3) & 0x1F;
				HCLEN = data[pos / 8 + 2] & 0xF;
				break;
			case 7:
				HLIT = (data[pos / 8] >> 7) + ((data[pos / 8 + 1] & 0xF) << 1);
				HDIST = (data[pos / 8 + 1] >> 4) + ((data[pos / 8 + 2] & 0x1) << 4);
				HCLEN = (data[pos / 8 + 2] >> 1) & 0xF;
				break;
			}

			pos += 14;

			HLIT += 257;
			HDIST += 1;
			HCLEN += 4;

			Log::debug("DEFLATE", "HLIT: %i, HDIST: %i, HCLEN: %i", HLIT, HDIST, HCLEN);

			u8 CLCodeLengths[19] = {};

			for (u32 i = 0; i < HCLEN; i++) {

				if (pos % 8 < 6) {
					CLCodeLengths[shuffle[i]] = (data[pos/8] >> (pos % 8)) & 0x7;
				} else if (pos % 8 == 6) {
					CLCodeLengths[shuffle[i]] = (data[pos/8] >> 6) + ((data[pos/8 + 1] & 0x1) << 2);
				} else {
					CLCodeLengths[shuffle[i]] = (data[pos/8] >> 7) + ((data[pos/8 + 1] & 0x3) << 1);
				}

				pos += 3;

			}

			for (u32 i = 0; i < 19; i++) {
				Log::debug("DEFLATE", "%2i: %i (%i)", i, CLCodeLengths[i], CLCodeLengths[shuffle[i]]);
			}

			u8 CLCodes[19] = {};

			u16 code = 0;
			u8 blCount[8] = {};
			u16 nextCode[8] = {};

			for (u8 CLCodeLength : CLCodeLengths) {
				blCount[CLCodeLength]++;
			}

			for (u32 bits = 0; bits < 7; bits++) {

				code = (code + blCount[bits + 1]) << 1;
				nextCode[bits + 1] = code;

			}

			for (u32 n = 0; n < 19; n++) {

				u32 len = CLCodeLengths[n];

				if (len != 0) {

					CLCodes[n] = nextCode[len - 1];
					nextCode[len - 1]++;

				}

			}

			break;

		}
		case 0:
		case 1:
		default:
			break;
		}

		last = true;

	}

	return { 0 };
}
