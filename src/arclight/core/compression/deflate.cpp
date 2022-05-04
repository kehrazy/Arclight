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

constexpr u8 rev7b[128] = {
		0b0000000, 0b1000000, 0b0100000, 0b1100000, 0b0010000, 0b1010000, 0b0110000, 0b1110000,
		0b0001000, 0b1001000, 0b0101000, 0b1101000, 0b0011000, 0b1011000, 0b0111000, 0b1111000,
		0b0000100, 0b1000100, 0b0100100, 0b1100100, 0b0010100, 0b1010100, 0b0110100, 0b1110100,
		0b0001100, 0b1001100, 0b0101100, 0b1101100, 0b0011100, 0b1011100, 0b0111100, 0b1111100,
		0b0000010, 0b1000010, 0b0100010, 0b1100010, 0b0010010, 0b1010010, 0b0110010, 0b1110010,
		0b0001010, 0b1001010, 0b0101010, 0b1101010, 0b0011010, 0b1011010, 0b0111010, 0b1111010,
		0b0000110, 0b1000110, 0b0100110, 0b1100110, 0b0010110, 0b1010110, 0b0110110, 0b1110110,
		0b0001110, 0b1001110, 0b0101110, 0b1101110, 0b0011110, 0b1011110, 0b0111110, 0b1111110,
		0b0000001, 0b1000001, 0b0100001, 0b1100001, 0b0010001, 0b1010001, 0b0110001, 0b1110001,
		0b0001001, 0b1001001, 0b0101001, 0b1101001, 0b0011001, 0b1011001, 0b0111001, 0b1111001,
		0b0000101, 0b1000101, 0b0100101, 0b1100101, 0b0010101, 0b1010101, 0b0110101, 0b1110101,
		0b0001101, 0b1001101, 0b0101101, 0b1101101, 0b0011101, 0b1011101, 0b0111101, 0b1111101,
		0b0000011, 0b1000011, 0b0100011, 0b1100011, 0b0010011, 0b1010011, 0b0110011, 0b1110011,
		0b0001011, 0b1001011, 0b0101011, 0b1101011, 0b0011011, 0b1011011, 0b0111011, 0b1111011,
		0b0000111, 0b1000111, 0b0100111, 0b1100111, 0b0010111, 0b1010111, 0b0110111, 0b1110111,
		0b0001111, 0b1001111, 0b0101111, 0b1101111, 0b0011111, 0b1011111, 0b0111111, 0b1111111
};

static u8 CLCodeLengths[19] = {};
static u8 CLCodeLUT[128];



void readCLCodes(std::span<const u8>& data, u64& pos, u32 numCodes, u8* CodeLengths) {
	for (u32 i = 0; i < numCodes; i++) {
		u32 code;

		if (pos + 7 >= data.size() * 8) {
			throw CompressorException("INFLATE input too small");
		}

		switch (pos % 8) {
		case 0:
			code = data[pos/8] & 0x7F;
			break;
		case 1:
			code = data[pos/8] >> 1;
			break;
		default:
			code = (data[pos/8] >> (pos % 8)) + ((data[pos/8 + 1] & ((1 << ((pos % 8) - 1)) - 1)) << (8 - (pos % 8)));
			break;
		}

		code = rev7b[code];

		pos += CLCodeLengths[CLCodeLUT[code]];

		u32 symbol = CLCodeLUT[code];
		u32 rLen = 0;

		if (symbol < 16) {
			CodeLengths[i] = symbol;
		} else if (symbol == 16) {

			if (pos + 2 >= data.size() * 8) {
				throw CompressorException("INFLATE input too small");
			}

			if (pos % 8 == 7) {

				rLen = data[(pos)/8] >> 6 | data[(pos) / 8 + 1];

				for (u32 j = 0; j < 3 + rLen; j++) {
					CodeLengths[i + j] = CodeLengths[i - 1];
				}

			} else {

				rLen = (data[pos/8] >> (pos % 8)) & 0x3;

				for (u32 j = 0; j < 3 + rLen; j++) {
					CodeLengths[i + j] = CodeLengths[i - 1];
				}

			}

			i += 2 + rLen;

			pos += 2;

		} else if (symbol == 17) {

			if (pos + 3 >= data.size() * 8) {
				throw CompressorException("INFLATE input too small");
			}

			if (pos % 8 < 6) {
				rLen = (data[pos/8] >> (pos % 8)) & 0x7;
			} else if (pos % 8 == 6) {
				rLen = (data[pos/8] >> 6) + ((data[pos/8 + 1] & 0x1) << 2);
			} else {
				rLen = (data[pos/8] >> 7) + ((data[pos/8 + 1] & 0x3) << 1);
			}

			pos += 3;

			i += 3 + rLen;

		} else if (symbol == 18) {

			if (pos + 7 >= data.size() * 8) {
				throw CompressorException("INFLATE input too small");
			}

			switch (pos % 8) {
			case 0:
				rLen = data[pos/8] & 0x7F;
				break;
			case 1:
				rLen = data[pos/8] >> 1;
				break;
			default:
				rLen = (data[pos/8] >> (pos % 8)) + ((data[pos/8 + 1] & ((1 << ((pos % 8) - 1)) - 1)) << (8 - (pos % 8)));
				break;
			}

			pos += 7;

			i += 10 + rLen;

		}

		ArcDebug() << code << CLCodeLUT[code] << rLen;

	}
	
}



std::vector<u8> Compress::inflate(std::span<const u8> data) {
	u64 pos = 0;
	bool last = false;

	if (data.empty()) {
		return {};
	}

	if (data.size() < 3) {
		throw CompressorException("INFLATE input too small");
	}

	while (!last) {
		if (pos >= data.size() * 8) {
			throw CompressorException("INFLATE input too small");
		}

		last = data[pos / 8] >> (pos % 8);
		pos++;

		if (pos + 2 >= data.size() * 8) {
			throw CompressorException("INFLATE input too small");
		}

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

			if (pos + 14 >= data.size() * 8) {
				throw CompressorException("INFLATE input too small");
			}

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

			// Read CL Code Lengths
			for (u32 i = 0; i < HCLEN; i++) {

				if (pos + 3 >= data.size() * 8) {
					throw CompressorException("INFLATE input too small");
				}

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

			// Generate CL Codes
			{

				u16 code = 0;
				u8 blCount[8] = {};
				u16 nextCode[8] = {};

				for (u8 CLCodeLength: CLCodeLengths) {
					blCount[CLCodeLength]++;
				}

				for (u32 bits = 0; bits < 7; bits++) {

					code = (code + blCount[bits + 1]) << 1;
					nextCode[bits + 1] = code;

				}

				for (u32 n = 0; n < 19; n++) {

					u32 len = CLCodeLengths[n];

					if (len != 0) {

						for (u32 i = 0; i < 1 << (7 - len); i++) {
							CLCodeLUT[(nextCode[len - 1] << (7 - len)) + i] = n;
						}
						nextCode[len - 1]++;

					}

				}

			}

			u8 LLCodeLengths[286] = {};
			u8 DCodeLengths[33] = {};

			// Use CL Codes to read LL and Distance Code Lengths
			readCLCodes(data, pos, HLIT, LLCodeLengths);
			readCLCodes(data, pos, HDIST, DCodeLengths);

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
