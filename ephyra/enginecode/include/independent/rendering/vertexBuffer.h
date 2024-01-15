/** \file vertexBuffer.h */
#pragma once

#include <cstdint>
#include "rendering/bufferLayout.h"

namespace Engine
{

	/** \class VertexBuffer
	*	API Agnostic Vertex Buffer Class
	*/

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
		virtual void edit(void* vertices, uint32_t size, uint32_t offset) = 0;
		virtual inline uint32_t getRenderID() const = 0;
		virtual inline const vertexBufferLayout& getLayout() const = 0;

		static VertexBuffer* create(void* vertices, uint32_t size, const vertexBufferLayout& layout);
	};

}
