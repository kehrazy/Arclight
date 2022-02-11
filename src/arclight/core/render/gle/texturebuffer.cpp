/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texturebuffer.cpp
 */

#include "texturebuffer.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


bool TextureBuffer::create() {

	if (!isCreated()) {

		glGenTextures(1, &texID);

		if (!texID) {
			texID = invalidID;
			return false;
		}
	}

	return Buffer::create();
}



void TextureBuffer::destroy() {

	if (isCreated()) {
		glDeleteTextures(1, &texID);
		texID = invalidID;
	}

	Buffer::destroy();
}



void TextureBuffer::load(TextureBufferFormat format) {

	gle_assert(isCreated(), "Texture buffer hasn't been created yet");
	gle_assert(!requiresExtension(format) || (threeComponentFormatSupported() && requiresExtension(format)), "Three component formats not supported");

	this->format = format;

	glTexBuffer(GL_TEXTURE_BUFFER, static_cast<u32>(format), id);
}



void TextureBuffer::loadRange(SizeT offset, SizeT size, TextureBufferFormat format) {

	gle_assert(rangeSupported(), "Texture buffer range not supported");
	gle_assert(isCreated(), "Texture buffer hasn't been created yet");
	gle_assert((offset + size) <= this->size, "Attempted to load out of bounds buffer data to texture (buffer object ID=%d)", id);

	this->format = format;

	glTexBufferRange(GL_TEXTURE_BUFFER, static_cast<u32>(format), id, offset, size);
}



void TextureBuffer::activate(u32 unit) {

	Texture::activateUnit(unit);

	glBindTexture(GL_TEXTURE_BUFFER, texID);
}



void TextureBuffer::bindImageUnit(u32 unit, Access access) {

	gle_assert(Texture::imageLoadStoreSupported(), "Cannot bind image unit, image load store not supported");
	gle_assert(Image::isImageUnitCompatible(format), "Texture %d has an image unit incompatible format (attempted to bind image unit)", texID);

	glBindImageTexture(unit, texID, 0, false, 0, static_cast<u32>(access), static_cast<u32>(format));
}



bool TextureBuffer::rangeSupported() {
	return GLE_EXT_SUPPORTED(ARB_texture_buffer_range);
}



bool TextureBuffer::threeComponentFormatSupported() {
	return GLE_EXT_SUPPORTED(ARB_texture_buffer_object_rgb32);
}



bool TextureBuffer::requiresExtension(TextureBufferFormat format) {

	switch (format) {

		case TextureBufferFormat::RGB32f:
		case TextureBufferFormat::RGB32i:
		case TextureBufferFormat::RGB32ui:
			return true;

		default:
			return false;

	}
}


GLE_END