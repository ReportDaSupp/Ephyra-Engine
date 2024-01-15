/** \file texture.h */
#pragma once

#include <cstdint>
#include "rendering/textureUnitManager.h"

namespace Engine
{

	/** \class Texture
	*	API Agnostic Texture
	*/

	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void edit(uint32_t xOffset, uint32_t yOffset, uint32_t width, uint32_t height, unsigned char* data) = 0;;
		virtual void load(TextureUnitManager& TextUM, uint32_t& unit) = 0;
		virtual inline uint32_t getID() = 0;
		virtual inline uint32_t getWidth() = 0;
		virtual inline uint32_t getHeight() = 0;

		virtual inline uint32_t getChannels() = 0;

		virtual inline uint32_t getWidthf() = 0;
		virtual inline uint32_t getHeightf() = 0;

		static Texture* create(const char* filepath);
		static Texture* create(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);

	private:
		
		virtual void init(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data) = 0;
	};
}
