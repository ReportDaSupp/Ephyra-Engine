/** \file indexBuffer.h */
#pragma once

#include <cstdint>

namespace Ephyra
{

	/** \class IndexBuffer
	*	API Agnostic Index Buffer
	*/

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual void edit(void* indices, uint32_t size, uint32_t offset) = 0;
		virtual inline uint32_t getRenderID() const = 0;
		virtual inline uint32_t getCount() const = 0;

		static IndexBuffer* create(uint32_t* indices, uint32_t count);
	};
}
