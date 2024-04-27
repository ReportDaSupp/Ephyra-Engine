/** \file OpenGLShader.cpp */

#include "Ephyra_pch.h"
#include "Platform/OpenGl/OpenGLTexture.h"
#include <glad/glad.h>
#include "Core/Systems/Utility/Log.h"
#include "Core/Rendering/API/Global/RendererCommon.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Engine
{

	OpenGLTexture::OpenGLTexture(const char* filepath)
	{
		m_filepath = filepath;

		int width, height, channels;
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 0);

		 init(width, height, channels, data);

		 stbi_image_free(data);

	}

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
	{

		init(width, height, channels, data);
		
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_OpenGl_ID);
	}

	void OpenGLTexture::edit(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, unsigned char* data)
	{
		glBindTexture(GL_TEXTURE_2D, m_OpenGl_ID);
		if (data)
		{
			if (m_channels == 0) glTexSubImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
			if (m_channels == 1) glTextureSubImage2D(m_OpenGl_ID, 0.f, xOffset, yOffset, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
			if (m_channels == 3) glTextureSubImage2D(m_OpenGl_ID, 0.f, xOffset, yOffset, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if (m_channels == 4) glTextureSubImage2D(m_OpenGl_ID, 0.f, xOffset, yOffset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}

	void OpenGLTexture::load(uint32_t unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_OpenGl_ID);
	}

	void OpenGLTexture::init(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
	{
		glGenTextures(1, &m_OpenGl_ID);
		glBindTexture(GL_TEXTURE_2D, m_OpenGl_ID);

		if (channels == 0) 
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);

		}
		else
		{	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
			else if (channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			else if (channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			else return;
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		m_width = width;
		m_height = height;
		m_channels = channels;
	}

}