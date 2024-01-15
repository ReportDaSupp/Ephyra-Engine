/** \file OpenGLUniformBuffer */
#pragma once

#include "rendering/uniformBuffer.h"

namespace Engine
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(const uniformBufferLayout& layout); //!< Constructor
		~OpenGLUniformBuffer();
		inline uint32_t getRenderID() override { return m_OpenGL_ID; }
		inline uniformBufferLayout getLayout() override { return m_layout; }
		void attachShaderBlock(const std::shared_ptr<Shader>& shader, const char* blockName) override;
		void uploadData(const char* uniformName, void* data) override;
		void bindUniformBuffer() override;

	private:
		uint32_t m_OpenGL_ID; //!< OpenGL ID
		static uint32_t s_blockNumber; //!< Accesible block number
	};
}
