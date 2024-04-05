/** \file OpenGLIndirectBuffer.cpp */

#include "Ephyra_pch.h"
#include "Platform/OpenGl/OpenGLIndirectBuffer.h"
#include <glad/glad.h>

namespace Engine
{


	
	OpenGLIndirectBuffer::OpenGLIndirectBuffer(DrawElementsIndirectCommand* commands, uint32_t count)
	{
		m_commands = commands;
		m_count = count;
		m_OpenGL_ID = NULL;
		glGenBuffers(1, &m_OpenGL_ID);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_OpenGL_ID);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, count, commands, GL_DYNAMIC_DRAW);

	}

	OpenGLIndirectBuffer::~OpenGLIndirectBuffer()
	{

	}

	void OpenGLIndirectBuffer::edit(DrawElementsIndirectCommand* commands, uint32_t count, uint32_t offset)
	{

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_OpenGL_ID);
		glBufferSubData(GL_DRAW_INDIRECT_BUFFER, offset, count * sizeof(DrawElementsIndirectCommand), commands);

	}

}