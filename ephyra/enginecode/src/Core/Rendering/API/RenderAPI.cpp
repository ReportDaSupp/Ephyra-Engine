/** \file renderAPI.cpp */

#include "Ephyra_pch.h"
#include "Core/Rendering/API/Global/RenderAPI.h"
#include "Core/Systems/Utility/Log.h"
#include "Platform/OpenGl/OpenGLIndexBuffer.h"
#include "Platform/OpenGl/OpenGLVertexBuffer.h"
#include "Platform/OpenGl/OpenGLShader.h"
#include "Platform/OpenGl/OpenGLTexture.h"
#include "Platform/OpenGl/OpenGLVertexArray.h"
#include "Platform/OpenGl/OpenGLUniformBuffer.h"
#include "Platform/OpenGl/OpenGLIndirectBuffer.h"
#include "Platform/OpenGl/OpenGLFrameBuffer.h"
#include <platform/OpenGl/OpenGLPostProcessing.h>



namespace Engine
{

	RenderAPI::API RenderAPI::s_API = RenderAPI::API::OpenGL;

	PostProcessing* postProcessor;

	PostProcessing* PostProcessing::create()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render API is Not Supported");
			return nullptr;
		case RenderAPI::API::OpenGL:
			if (!postProcessor)
			{
				postProcessor = new OpenGLPostProcessing();
			}
			return postProcessor;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}
	}

	FrameBuffer* FrameBuffer::create() 
	{
		switch (RenderAPI::getAPI()) 
		{
		case RenderAPI::API::None:
			Log::error("No Render API is Not Supported");
			return nullptr;
		case RenderAPI::API::OpenGL:
			return new OpenGLFrameBuffer();
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t count)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLIndexBuffer(indices, count);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	VertexBuffer* VertexBuffer::create(void* vertices, uint32_t size, const vertexBufferLayout& layout)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLVertexBuffer(vertices, size, layout);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	VertexArray* VertexArray::create()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLVertexArray();
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	Shader* Shader::create(const char* vertexFilepath, const char* fragmentFilepath)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLShader(vertexFilepath, fragmentFilepath);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	Shader* Shader::create(const char* filepath)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLShader(filepath);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	Texture* Texture::create(const char* filepath)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLTexture(filepath);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	Texture* Texture::create(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLTexture(width, height, channels, data);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}

	UniformBuffer* UniformBuffer::create(const uniformBufferLayout& layout)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLUniformBuffer(layout);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;
	}

	IndirectBuffer* IndirectBuffer::create(DrawElementsIndirectCommand* commands, uint32_t count)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::None:
			Log::error("No Render Api is Not Supported");
			break;
		case RenderAPI::API::OpenGL:
			return new OpenGLIndirectBuffer(commands, count);
			break;
		case RenderAPI::API::Direct3D:
			Log::error("Direct3D is Not Supported");
			break;
		case RenderAPI::API::Vulkan:
			Log::error("Vulkan is Not Supported");
			break;
		}

		return nullptr;

	}
}