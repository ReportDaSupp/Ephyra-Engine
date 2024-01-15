/** \file vertexArray.h */
#pragma once

#include "rendering/bufferLayout.h"

#include <vector>
#include <memory>

#include "indexBuffer.h"
#include "vertexBuffer.h"
namespace Engine
{

	/** \class VertexArray
	*	API Agnostic Vertex Array Class
	*/

	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;
		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
		virtual void bindIndexBuffer() = 0;
		virtual std::shared_ptr<IndexBuffer> getIndexBuffer() = 0;
		virtual std::vector < std::shared_ptr<VertexBuffer>> getVertexBuffer() = 0;
		virtual inline uint32_t getDrawCount() = 0;
		virtual inline uint32_t getRenderID() const = 0;

		static VertexArray* create();
	};
}