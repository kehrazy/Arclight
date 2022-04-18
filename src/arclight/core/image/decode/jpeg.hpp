/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 jpeg.hpp
 */

#pragma once

#include "memory/alignedallocator.hpp"
#include "types.hpp"

#include <array>
#include <vector>
#include <unordered_map>



namespace JPEG {

	namespace Markers {

		constexpr u16 SOF0 = 0xFFC0;
		constexpr u16 SOF1 = 0xFFC1;
		constexpr u16 SOF2 = 0xFFC2;
		constexpr u16 SOF3 = 0xFFC3;
		constexpr u16 DHT = 0xFFC4;
		constexpr u16 SOF5 = 0xFFC5;
		constexpr u16 SOF6 = 0xFFC6;
		constexpr u16 SOF7 = 0xFFC7;
		constexpr u16 JPG = 0xFFC8;
		constexpr u16 SOF9 = 0xFFC9;
		constexpr u16 SOF10 = 0xFFCA;
		constexpr u16 SOF11 = 0xFFCB;
		constexpr u16 SOF13 = 0xFFCD;
		constexpr u16 SOF14 = 0xFFCE;
		constexpr u16 SOF15 = 0xFFCF;
		constexpr u16 SOI = 0xFFD8;
		constexpr u16 EOI = 0xFFD9;
		constexpr u16 SOS = 0xFFDA;
		constexpr u16 DQT = 0xFFDB;
		constexpr u16 DRI = 0xFFDD;
		constexpr u16 APP0 = 0xFFE0;
		constexpr u16 APP1 = 0xFFE1;

	};

	enum class FrameType {
		Baseline,
		ExtendedSequential,
		Progressive,
		Lossless
	};

	enum class Encoding {
		Huffman,
		Arithmetic
	};

	constexpr u8 dezigzagTable[64] = {
		 0,  1,  8, 16,  9,  2,  3, 10,
		17, 24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13,  6,  7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63
	};

	constexpr u8 dezigzagTableTransposed[64] = {
		 0,  8,  1,  2,  9, 16, 24, 17,
		10,  3,  4, 11, 18, 25, 32, 40,
		33, 26, 19, 12,  5,  6, 13, 20,
		27, 34, 41, 48, 56, 49, 42, 35,
		28, 21, 14,  7, 15, 22, 29, 36,
		43, 50, 57, 58, 51, 44, 37, 30,
		23, 31, 38, 45, 52, 59, 60, 53,
		46, 39, 47, 54, 61, 62, 55, 63
	};

	constexpr u8 zigzagIndexTransposed[64] = {
		 0,  2,  3,  9, 10, 20, 21, 35,
		 1,  4,  8, 11, 19, 22, 34, 36,
		 5,  7, 12, 18, 23, 33, 37, 48,
		 6, 13, 17, 24, 32, 38, 47, 49,
		14, 16, 25, 31, 39, 46, 50, 57,
		15, 26, 30, 40, 45, 51, 56, 58,
		27, 29, 41, 44, 52, 55, 59, 62,
		28, 42, 43, 53, 54, 60, 61, 63
	};

	constexpr u8 jfifString[5] = {0x4A, 0x46, 0x49, 0x46, 0x00};
	constexpr u8 jfxxString[5] = {0x4A, 0x46, 0x58, 0x58, 0x00};


	struct QuantizationTable {

		std::array<i32, 64> data;
		bool hasData;

		QuantizationTable() { reset(); }

		constexpr bool empty() const noexcept {
			return !hasData;
		}

		void reset() {

			std::fill(data.begin(), data.end(), 0);
			hasData = false;

		}

	};


	using HuffmanResult = std::pair<u8, u8>;

	struct HuffmanTable {

		constexpr static HuffmanResult defaultHuffmanResult = {0xC, 0x1};

		HuffmanTable() { reset(); }

		constexpr bool empty() const noexcept {
			return !maxLength;
		}

		void reset() {

			maxLength = 0;
			std::fill(fastTable.begin(), fastTable.end(), defaultHuffmanResult);
			extTables.clear();

		}

		u32 maxLength;
		std::array<HuffmanResult, 256> fastTable;
		std::vector<std::vector<HuffmanResult>> extTables;

	};

	struct FrameComponent {

		constexpr FrameComponent() noexcept : FrameComponent(0, 0, 0) {}
		constexpr FrameComponent(u32 sx, u32 sy, u32 qTableID) noexcept : samplesX(sx), samplesY(sy), qID(qTableID), width(0), height(0), progression(0) {}

		u32 samplesX, samplesY;
		u32 qID;

		u32 width, height;

		u32 progression;
		std::vector<i32> progressiveBuffer;
		std::vector<i16> imageData;


	};

	struct Frame {

		Frame() : type(FrameType::Baseline), differential(false), encoding(Encoding::Huffman), bits(8), lines(0), samples(1) {}

		FrameType type;
		bool differential;
		Encoding encoding;

		u32 bits;
		u32 lines;
		u32 samples;

		std::unordered_map<u8, FrameComponent> components;

	};

	struct ScanComponent {

		constexpr ScanComponent(HuffmanTable& dct, HuffmanTable& act, QuantizationTable& qt, FrameComponent& component)
			: dcTable(dct), acTable(act), qTable(qt), frameComponent(component), prediction(0), block(nullptr) {}

		HuffmanTable& dcTable;
		HuffmanTable& acTable;
		QuantizationTable& qTable;
		FrameComponent& frameComponent;

		i32 prediction;
		i32* block;

	};

	struct Scan {

		constexpr Scan() noexcept : spectralStart(0), spectralEnd(0), approximationHigh(0), approximationLow(0), maxSamplesX(0), maxSamplesY(0), mcuDataUnits(0), totalMCUs(0), mcusX(0), mcusY(0) {}

		std::vector<ScanComponent> scanComponents;
		u32 spectralStart;
		u32 spectralEnd;
		u32 approximationHigh;
		u32 approximationLow;

		u32 maxSamplesX, maxSamplesY;
		u32 mcuDataUnits;
		u32 mcusX, mcusY;
		u32 totalMCUs;

	};

	struct Progression {



	};

}