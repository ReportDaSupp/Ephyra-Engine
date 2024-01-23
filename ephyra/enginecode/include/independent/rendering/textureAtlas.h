/** \file Texture Atlas.h*/
#pragma once

#include <vector>
#include "rendering/subTexture.h"

namespace Ephyra
{
	struct SimpleRect { uint32_t x, y, w, h; };
	class TextureAtlas
	{
	public:
		TextureAtlas(glm::ivec2 size = { 4096, 4096 }, uint32_t channels = 4, uint32_t reserveSpace = 128); //!< Constructor, Takes Size And Channels
		bool add(const char * filepath, SubTexture& result); //!< Attempt To Add With A Filename
		bool add(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, SubTexture& result); //!< Attempt To Add By Data
		inline uint32_t getChannels() const { return m_baseTexture->getChannels(); }
		inline std::shared_ptr<Texture> getBaseTexture() const { return m_baseTexture; };
		inline uint32_t getID() const { return m_baseTexture->getID(); };
	private:
		std::vector<SimpleRect> m_spaces;
		std::shared_ptr<Texture> m_baseTexture; //!< Texture Which Holds All Sub Textured Pixel Data
	};
}
