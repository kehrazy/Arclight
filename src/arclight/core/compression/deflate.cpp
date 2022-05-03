/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 deflate.cpp
 */

#include "deflate.hpp"
#include "util/log.hpp"



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
			u8 HLIT, HDIST, HCLEN;

			switch (pos % 8) {
			case 0:
				HLIT = data[pos / 8] & 0x1F;
				HDIST = (data[pos / 8] >> 5) + ((data[pos / 8 + 1] & 0x3) << 3);
				HCLEN = (data[pos / 8 + 1] >> 2) & 0x1F;
				pos += 15;
				break;
			case 1:
				HLIT = (data[pos / 8] >> 1) & 0x1F;
				HDIST = (data[pos / 8] >> 6) + ((data[pos / 8 + 1] & 0x7) << 2);
				HCLEN = data[pos / 8 + 1] >> 3;
				pos += 15;
				break;
			case 2:
				HLIT = (data[pos / 8] >> 2) & 0x1F;
				HDIST = (data[pos / 8] >> 7) + ((data[pos / 8 + 1] & 0xF) << 1);
				HCLEN = (data[pos / 8 + 1] >> 4) + ((data[pos / 8 + 2] & 0x1) << 4);
				pos += 15;
				break;
			case 3:
				HLIT = data[pos / 8] >> 3;
				HDIST = data[pos / 8 + 1] & 0x1F;
				HCLEN = (data[pos / 8 + 1] >> 5) + ((data[pos / 8 + 2] & 0x3) << 3);
				pos += 15;
				break;
			case 4:
				HLIT = (data[pos / 8] >> 4) + ((data[pos / 8 + 1] & 0x1) << 4);
				HDIST = (data[pos / 8 + 1] >> 1) & 0x1F;
				HCLEN = (data[pos / 8 + 1] >> 6) + ((data[pos / 8 + 2] & 0x7) << 2);
				pos += 15;
				break;
			case 5:
				HLIT = (data[pos / 8] >> 5) + ((data[pos / 8 + 1] & 0x3) << 3);
				HDIST = (data[pos / 8 + 1] >> 2) & 0x1F;
				HCLEN = (data[pos / 8 + 1] >> 7) + ((data[pos / 8 + 2] & 0xF) << 1);
				pos += 15;
				break;
			case 6:
				HLIT = (data[pos / 8] >> 6) + ((data[pos / 8 + 1] & 0x7) << 2);
				HDIST = (data[pos / 8 + 1] >> 3) & 0x1F;
				HCLEN = data[pos / 8 + 2] & 0x1F;
				pos += 15;
				break;
			case 7:
				HLIT = (data[pos / 8] >> 7) + ((data[pos / 8 + 1] & 0xF) << 1);
				HDIST = (data[pos / 8 + 1] >> 4) + ((data[pos / 8 + 2] & 0x1) << 4);
				HCLEN = (data[pos / 8 + 2] >> 1) & 0x1F;
				pos += 15;
				break;
			}

			Log::debug("DEFLATE", "HLIT: %i, HDIST: %i, HCLEN: %i", HLIT, HDIST, HCLEN);

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
