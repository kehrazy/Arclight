/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 pngdecoder.hpp
 */

#pragma once

#include "decoder.hpp"
#include "bitmap.hpp"
#include "image/image.hpp"
#include "stream/binaryreader.hpp"



class PNGDecoder : public IImageDecoder {

public:

	PNGDecoder() : validDecode(false) {}

	void decode(std::span<const u8> data);

	template<Pixel P>
	Image<P> getImage() {

		if (!validDecode) {
			throw ImageDecoderException("Bad image decode");
		}

		return Image<P>::fromRaw(image);

	}

private:

	BinaryReader reader;
	RawImage image;
	bool validDecode;

	u32 width;
	u32 height;
	u8 bitDepth;
	u8 colorType;
	u8 compressionMethod;
	u8 filterMethod;
	u8 interlaceMethod;

	PixelRGB8 palette[256];

};