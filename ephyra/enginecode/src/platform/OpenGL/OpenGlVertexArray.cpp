/** \file OpenGLVertexArray.cpp */

#include "engine_pch.h"

#include <glad/glad.h>
#include "platform/OpenGL/OpenGLVertexArray.h"
#include "systems/log.h"

namespace Engine
{
	namespace SDT
	{
		static GLenum toGLType(ShaderDataType type)
		{
			switch (type)
			{
			case (ShaderDataType::FlatByte): return GL_BYTE;
			case (ShaderDataType::FlatInt): return GL_INT;
			case (ShaderDataType::Byte4): return GL_UNSIGNED_BYTE;
			case (ShaderDataType::Short): return GL_SHORT;
			case (ShaderDataType::Short2): return GL_SHORT;
			case (ShaderDataType::Short3): return GL_SHORT;
			case (ShaderDataType::Short4): return GL_SHORT;
			case (ShaderDataType::Float): return GL_FLOAT;
			case (ShaderDataType::Float2): return GL_FLOAT;
			case (ShaderDataType::Float3): return GL_FLOAT;
			case (ShaderDataType::Float4): return GL_FLOAT;
			case (ShaderDataType::Mat4): return GL_FLOAT;
			case (ShaderDataType::Int): return GL_INT;
			default: return GL_INVALID_ENUM;
			}
		}
	}
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_OpenGL_ID);
		glBindVertexArray(m_OpenGL_ID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_OpenGL_ID);
	}

	void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		m_vertexBuffer.push_back(vertexBuffer);

		glBindVertexArray(m_OpenGL_ID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getRenderID());
		const auto& layout = vertexBuffer->getLayout();
		for (const auto& element : layout)
		{
			uint32_t normalised = GL_FALSE;
			if (element.m_normalised) { normalised = GL_TRUE; }

			if (element.m_dataType == ShaderDataType::FlatByte || element.m_dataType == ShaderDataType::FlatInt)
			{
				glEnableVertexAttribArray(m_attributeIndex);
				glVertexAttribIPointer(m_attributeIndex, 
					SDT::componentCount(element.m_dataType), 
					SDT::toGLType(element.m_dataType), 
					layout.getStride(), 
					(const void*)element.m_offset);

				Log::info("{0}, {1}, {2}, {3}, {4}, {5}, {6}",
					m_attributeIndex, 
					SDT::componentCount(element.m_dataType), 
					SDT::toGLType(element.m_dataType), 
					element.m_normalised ? GL_TRUE : GL_FALSE, 
					layout.getStride(), 
					element.m_offset, 
					element.m_instanceDivisor);
				glVertexAttribDivisor(m_attributeIndex, element.m_instanceDivisor);
				m_attributeIndex++;
			}
			else if (element.m_dataType == ShaderDataType::Mat4)
			{
				uint8_t count = SDT::componentCount(element.m_dataType);
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_attributeIndex);
					glVertexAttribPointer(m_attributeIndex, 
						count, 
						SDT::toGLType(element.m_dataType), 
						(element.m_normalised ? GL_TRUE : GL_FALSE), 
						layout.getStride(), 
						(const void*)(sizeof(float) * count * i));

					Log::info("{0}, {1}, {2}, {3}, {4}, {5}, {6}",
						m_attributeIndex, 
						SDT::componentCount(element.m_dataType), 
						SDT::toGLType(element.m_dataType), 
						element.m_normalised ? GL_TRUE : GL_FALSE, 
						layout.getStride(), 
						sizeof(float) * count * i, 
						element.m_instanceDivisor);
					glVertexAttribDivisor(m_attributeIndex, element.m_instanceDivisor);
					m_attributeIndex++;
				}
			}
			else
			{
				glEnableVertexAttribArray(m_attributeIndex);
				glVertexAttribPointer(m_attributeIndex, 
					SDT::componentCount(element.m_dataType), 
					SDT::toGLType(element.m_dataType), 
					normalised, 
					layout.getStride(), 
					(void*)element.m_offset);

				Log::info("{0}, {1}, {2}, {3}, {4}, {5}, {6}",
					m_attributeIndex, 
					SDT::componentCount(element.m_dataType), 
					SDT::toGLType(element.m_dataType), 
					element.m_normalised ? GL_TRUE : GL_FALSE, 
					layout.getStride(), 
					element.m_offset,
					element.m_instanceDivisor);
				glVertexAttribDivisor(m_attributeIndex, element.m_instanceDivisor);
				m_attributeIndex++;
			}
		}
		//m_attributeIndex = 0;
	}

	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		m_indexBuffer = indexBuffer;
	}

	void OpenGLVertexArray::bindIndexBuffer()
	{
		glBindVertexArray(m_OpenGL_ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->getRenderID());
	}

}