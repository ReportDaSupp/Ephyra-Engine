/** \file OpenGLIndirectBuffer.h */
#pragma once

#include "Core/Rendering/API/Buffers/IndirectBuffer.h"

namespace Engine
{
	class OpenGLIndirectBuffer : public IndirectBuffer
	{
	public:
		OpenGLIndirectBuffer(DrawElementsIndirectCommand* commands, uint32_t count);
		virtual ~OpenGLIndirectBuffer();
		virtual inline uint32_t getID() const override { return m_OpenGL_ID; };
		virtual inline uint32_t getCount() const override { return m_count; };
		virtual void edit(DrawElementsIndirectCommand* commands, uint32_t count, uint32_t offset) override;

	private:
		DrawElementsIndirectCommand* m_commands;
		uint32_t m_count;	
		uint32_t m_OpenGL_ID;
	};
}