/** \file OpenGLVertexArray.h */
#pragma once

#include "Core/Rendering/API/Buffers/VertexArray.h"

namespace Engine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();
		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
		virtual void bindIndexBuffer() override;
		virtual std::shared_ptr<IndexBuffer> getIndexBuffer() override { return m_indexBuffer; };
		virtual std::vector<std::shared_ptr<VertexBuffer>> getVertexBuffer() override { return m_vertexBuffer; };
		virtual inline uint32_t getDrawCount() override { if (m_indexBuffer) { return m_indexBuffer->getCount(); } else { return 0; } }
		virtual inline uint32_t getRenderID() const override { return m_OpenGL_ID; }
	private:
		uint32_t m_OpenGL_ID; //!< Render ID
		uint32_t m_attributeIndex = 0; //!< Attribute Index
		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};
}
