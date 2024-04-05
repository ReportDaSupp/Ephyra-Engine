/** \file subTexture.h */
#pragma once

#include "Core/Resources/Utility/TextureUnitManager.h"
#include "Core/Rendering/API/Textures/Texture.h"

#include <memory>
#include <glm/glm.hpp>

namespace Engine
{

	class SubTexture
	{
	public:
		SubTexture() {};
		SubTexture(const std::shared_ptr<Texture>& texture, const glm::vec2& UVStart, const glm::vec2& UVEnd);

		inline glm::vec2 getUVStart() const { return m_UVStart; };
		inline glm::vec2 getUVEnd() const { return m_UVEnd; };
		inline glm::ivec2 getSize() const { return m_size; };
		inline uint32_t getTextureID() const { return m_texture->getID(); };
		glm::vec2 getSizef() const { return { static_cast<float>(m_size.x), static_cast<float>(m_size.y) }; };

		inline uint32_t getWidth() const { return m_size.x; }
		inline uint32_t getHeight() const { return m_size.y; }

		inline uint32_t getWidthf() const { return static_cast<float>(m_size.x); } //!< Get Floating Point Width
		inline uint32_t getHeightf() const { return static_cast<float>(m_size.y); } //!< Get Floating Point Height

		float transformU(float U); //!< Transform U to atlas U
		float transformV(float V); //!< Transform V to atlas V

		glm::vec2 transformUV(glm::vec2 UV); //!< Transform UV to atlas UV

		inline const std::shared_ptr<Texture>& getBaseTexture() const { return m_texture; }

	private:
		std::shared_ptr<Texture> m_texture;
		glm::vec2 m_UVStart;
		glm::vec2 m_UVEnd;
		glm::ivec2 m_size; //!< Size in pixels
	};

}
