/** \file OpenGLTexture.h */
#pragma once

#include "rendering/texture.h"

namespace Engine
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const char* filepath);
		OpenGLTexture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);
		virtual ~OpenGLTexture() override;
		virtual void edit(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, unsigned char* data) override;
		virtual void load(TextureUnitManager& TextUM, uint32_t& unit);
		virtual inline uint32_t getID() override { return m_OpenGl_ID; }
		virtual inline uint32_t getWidth() override { return m_width; }
		virtual inline uint32_t getHeight() override { return m_height; }
	
		virtual inline uint32_t getChannels() override { return m_channels; }

		virtual inline uint32_t getWidthf() override { return static_cast<float>(m_width); }
		virtual inline uint32_t getHeightf() override { return static_cast<float>(m_height); }

	private:
		uint32_t m_OpenGl_ID;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_channels;

		virtual void init(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data) override;
	};
}
