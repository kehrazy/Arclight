/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 pngdecoder.cpp
 */

#include "pngdecoder.hpp"
#include "math/math.hpp"



constexpr static u8 bitCountLUT[256] {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};



void PNGDecoder::decode(std::span<const u8> data) {

	validDecode = false;

	reader = BinaryReader(data, ByteOrder::Big);

	Image<Pixel::RGBA8> bufImage;

	if (reader.remainingSize() < 8) {
		throw ImageDecoderException("PNG stream size too small");
	}

	u64 magic = reader.read<u64>();

	if (magic != 0x89504e470d0a1a0a) {
		throw ImageDecoderException("PNG magic doesn't match");
	}

	while (true) {

		if (reader.remainingSize() < 8) {
			throw ImageDecoderException("PNG stream size too small");
		}

		u32 len = reader.read<u32>();
		u32 type = reader.read<u32>();

		if (len > (1 << 31)) {
			throw ImageDecoderException("PNG Chunk data field to long");
		}

		if (reader.remainingSize() < len + 4) {
			throw ImageDecoderException("PNG stream size too small");
		}

		switch (type) {

			// IHDR
			case 0x49484452:

				width = reader.read<u32>();
				height = reader.read<u32>();
				bitDepth = reader.read<u8>();
				colorType = reader.read<u8>();
				compressionMethod = reader.read<u8>();
				filterMethod = reader.read<u8>();
				interlaceMethod = reader.read<u8>();

				bufImage.resize(ImageScaling::Nearest, width, height);

				if (bitCountLUT[bitDepth] == 1) {

					switch (colorType) {

						case 0:

							if (bitDepth > 16) {

							}

					}

				} else {
					throw ImageDecoderException("PNG Faulty bit depth");
				}

				break;

		}

		break;

	}

}