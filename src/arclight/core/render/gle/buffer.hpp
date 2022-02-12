/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 buffer.hpp
 */

#pragma once

#include "glecore.hpp"
#include "globject.hpp"


GLE_BEGIN

enum class BufferType {
	VertexBuffer			= 0x8892, // GL_ARRAY_BUFFER
	ElementBuffer			= 0x8893, // GL_ELEMENT_ARRAY_BUFFER
	TransformFeedbackBuffer	= 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
	UniformBuffer			= 0x8A11, // GL_UNIFORM_BUFFER
	CopyReadBuffer			= 0x8F36, // GL_COPY_READ_BUFFER
	CopyWriteBuffer			= 0x8F37, // GL_COPY_WRITE_BUFFER
	ShaderStorageBuffer		= 0x90D2, // GL_SHADER_STORAGE_BUFFER
	PixelPackBuffer			= 0x88EB, // GL_PIXEL_PACK_BUFFER
	PixelUnpackBuffer		= 0x88EC, // GL_PIXEL_UNPACK_BUFFER
	TextureBuffer			= 0x8C2A, // GL_TEXTURE_BUFFER
};


enum class BufferAccess {
	StaticDraw	= 0x88E4, // GL_STATIC_DRAW
	DynamicDraw	= 0x88E8, // GL_DYNAMIC_DRAW
	StreamDraw	= 0x88E0, // GL_STREAM_DRAW
	StaticRead	= 0x88E5, // GL_STATIC_READ
	DynamicRead	= 0x88E9, // GL_DYNAMIC_READ
	StreamRead	= 0x88E1, // GL_STREAM_READ
	StaticCopy	= 0x88E6, // GL_STATIC_COPY
	DynamicCopy	= 0x88EA, // GL_DYNAMIC_COPY
	StreamCopy	= 0x88E2, // GL_STREAM_COPY
};


class Buffer : public GLObject {

public:

	//Creates a buffer if none has been created yet
	virtual bool create() override;

	//Destroys a buffer if it was created once
	virtual void destroy() override;

	//Allocates the buffer's storage. Required for (re-)allocation.
	void allocate(SizeT size, BufferAccess access = BufferAccess::StaticDraw);
	void allocate(SizeT size, const void* data, BufferAccess access = BufferAccess::StaticDraw);

	//Updates the buffer's data. Fails if no storage has been allocated first.
	void update(SizeT offset, SizeT size, const void* data);

	//Maps/Unmaps the buffer. Fails if buffer has been already mapped/unmapped.
	void* map(Access access);
	void unmap();

	//Unbinds the buffer. Fails if buffer hasn't been bound.
	void unbind();

	//Copies this buffer (or a portion of it) to destBuffer
	void copy(Buffer& destBuffer);
	void copy(Buffer& destBuffer, SizeT srcOffset, SizeT destOffset, SizeT size);

	//Checks the given states
	bool isBound() const;
	bool isInitialized() const;
	bool isMapped() const;

	SizeT getSize() const;

protected:

	//Protected so that only a specialized type may be created
	constexpr Buffer(BufferType type) : type(type), size(0), mapped(false) {}

	//Binds the buffer to the given target if not already. Fails if it hasn't been created yet.
	void bind(BufferType type);

	static inline u32 getBoundArrayIndex(BufferType type) {

		switch (type) {

			case BufferType::VertexBuffer:
				return 0;

			case BufferType::ElementBuffer:
				return 1;

			case BufferType::TransformFeedbackBuffer:
				return 2;

			case BufferType::UniformBuffer:
				return 3;

			case BufferType::CopyReadBuffer:
				return 4;

			case BufferType::CopyWriteBuffer:
				return 5;

			case BufferType::ShaderStorageBuffer:
				return 6;

			case BufferType::PixelPackBuffer:
				return 7;

			case BufferType::PixelUnpackBuffer:
				return 8;

			case BufferType::TextureBuffer:
				return 9;

			default:
				gle_force_assert("Illegal buffer type &d", type);
				return -1;

		}
	}

	static inline void setBoundBufferID(BufferType type, u32 id) {
		boundBufferIDs[getBoundArrayIndex(type)] = id;
	}

	static inline u32 getBoundBufferID(BufferType type) {
		return boundBufferIDs[getBoundArrayIndex(type)];
	}

	BufferType type;	//Currently bound type
	SizeT size;			//Buffer size or 0 if none has been allocated
	bool mapped;		//Defines if a buffer is currently mapped

private:

	//Active buffer handles per type
	static inline u32 boundBufferIDs[10] = {
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID
	};

};

GLE_END