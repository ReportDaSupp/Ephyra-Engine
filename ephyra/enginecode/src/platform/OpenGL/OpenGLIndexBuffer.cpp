/** \file OpenGLIndexBuffer.cpp */

#include "engine_pch.h"
#include <glad/glad.h>
#include "platform/OpenGl/OpenGLIndexBuffer.h"

namespace Engine
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_count(count)
	{
		glCreateBuffers(1, &m_OpenGL_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_OpenGL_ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_OpenGL_ID);
	}

	void OpenGLIndexBuffer::edit(void* indices, uint32_t count, uint32_t offsetIndex)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_OpenGL_ID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetIndex * sizeof(uint32_t), count * sizeof(uint32_t), indices);
	}

}