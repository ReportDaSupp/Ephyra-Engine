/** \file uniformBuffer.h */
#pragma once

#include <ctype.h>

#include <memory>
#include "bufferLayout.h"
#include "rendering/shader.h"
#include <unordered_map>

namespace Ephyra
{
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;
		virtual uint32_t getRenderID() = 0;
		virtual uniformBufferLayout getLayout() = 0;
		virtual void attachShaderBlock(const std::shared_ptr<Shader>& shader, const char* blockName) = 0;
		virtual void uploadData(const char* uniformName, void* data) = 0;
		virtual void bindUniformBuffer() = 0;
		static UniformBuffer* create(const uniformBufferLayout& layout);
	protected:
		uniformBufferLayout m_layout; //!< Uniform Buffer
		std::unordered_map<const char*, std::pair<uint32_t, uint32_t>> m_uniformCache; //!< Stores uniform names with offset and sizes
		uint32_t m_blockNumber; //!< Block number for Ubo
	};
}
