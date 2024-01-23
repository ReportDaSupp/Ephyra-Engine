/** \file OpenGLVertexBuffer.h */
#pragma once

#include "rendering/vertexBuffer.h"

namespace Ephyra
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(void* vertices, uint32_t size, vertexBufferLayout layout);
		virtual ~OpenGLVertexBuffer();
		virtual void edit(void* vertices, uint32_t size, uint32_t offset) override;
		virtual inline uint32_t getRenderID() const override { return m_OpenGL_ID; }
		virtual inline const vertexBufferLayout& getLayout() const override { return m_layout; }

	private:
		uint32_t m_OpenGL_ID; //!< Render ID
		vertexBufferLayout m_layout;
	};
}
