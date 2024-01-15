/** \file textureAtlas.cpp */

#include "engine_pch.h"
#include "rendering/textureAtlas.h"
#include <algorithm>
#include <stb_image.h>

namespace Engine
{
	TextureAtlas::TextureAtlas(glm::ivec2 size, uint32_t channels, uint32_t reserveSpace)
	{
		m_baseTexture.reset(Texture::create(size.x, size.y, channels, nullptr));

		m_spaces.reserve(reserveSpace);
		m_spaces.push_back({ 0, 0, (uint32_t)size.x, (uint32_t)size.y });
	}

	bool TextureAtlas::add(const char* filepath, SubTexture& result)
	{
		int32_t width, height, channels;
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, static_cast<int>(getChannels()));

		if (data) return(add(width, height, channels, data, result));

		stbi_image_free(data);
		return false;
	}

	bool TextureAtlas::add(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, SubTexture& result)
	{
		if (channels != getChannels()) return false;

		for (auto iterator = m_spaces.begin(); iterator != m_spaces.end(); ++iterator)
		{
			auto& space = *iterator;

			// Does The Texture Fit This Space
			if (width < space.w && height < space.h)
			{
				// Texture Fits! Add Texture Data
				m_baseTexture->edit(space.x, space.y, width, height, data);

				// Set SubTexture Result
				glm::vec2 UVStart( static_cast<float>(space.x) / m_baseTexture->getWidthf(), static_cast<float>(space.y) / m_baseTexture->getHeightf());
				glm::vec2 UVEnd(static_cast<float>(space.x + width) / m_baseTexture->getWidthf(), static_cast<float>(space.y+height) / m_baseTexture->getHeightf());
				result = SubTexture(m_baseTexture, UVStart, UVEnd);

					// Sort Out Remaining Spaces

				// Case 1: Texture Matches Space Size. Delete Space
				if (width == space.w && height == space.h)
				{
					m_spaces.erase(iterator);
					return true;
				}

				// Case 2: Width Matches Space Width, Height Doesnt Match. Split Space Horizontally In Two
				else if (width == space.w && height != space.h)
				{
					space.y += height;
					space.h -= height;
					return true;
				}

				// Case 3: Height Matches Space Height, Width Doesnt Match. Split Space Vertically In Two
				else if (width != space.w && height == space.h)
				{
					space.x += width;
					space.w -= width;
					return true;
				}

				// Case 4: Both Height And Width Less Than Space. Split Horizontally And Vertically
				else if (width != space.w && height != space.h)
				{
					SimpleRect newRect({ space.x, space.y + height, width, space.h - height });
					space.x += width;
					space.w -= width;
					m_spaces.push_back(newRect);

					std::sort(m_spaces.begin(), m_spaces.end(), [](SimpleRect& a, SimpleRect& b) { return a.w < b.w; });
					return true;
				}

			}
		}

		return false;
	}

}