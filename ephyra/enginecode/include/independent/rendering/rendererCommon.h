/**\file rendererCommon.h*/
#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "rendering/shaderDataType.h"
#include "rendering/texture.h"
#include "rendering/subTexture.h"
#include "rendering/textureUnitManager.h"
#include "rendering/renderCommands.h"
#include "rendering/uniformBuffer.h"
#include "rendering/shader.h"
#include "rendering/renderAPI.h"
#include "rendering/textureAtlas.h"

namespace Engine
{
	using SceneWideUniforms = std::unordered_map<const char*, std::pair<ShaderDataType, void*>>;

	class RendererCommon
	{
	public:
		static void actionCommand(std::shared_ptr<RenderCommand>& command) { command->m_action(); }
		inline static std::shared_ptr<TextureUnitManager> m_textUM;
		inline static std::shared_ptr<Texture> defaultTexture;
		inline static SubTexture defaultSubTexture;
		inline static std::shared_ptr<glm::vec4> defaultTint;

		inline static std::shared_ptr <std::array<int32_t, 32>> textureUnits;
		inline static std::vector<glm::vec3> lightColour;
		inline static std::vector<glm::vec3> lightPos;

		static uint32_t pack(const glm::vec4& tint) 
		{
				uint32_t result = 0;

				uint32_t r = (static_cast<uint32_t>(tint.r * 255.f) << 0);
				uint32_t g = (static_cast<uint32_t>(tint.g * 255.f) << 8);
				uint32_t b = (static_cast<uint32_t>(tint.b * 255.f) << 16);
				uint32_t a = (static_cast<uint32_t>(tint.a * 255.f) << 24);

				result = (r | g | b | a);
				return result;
		};
	};

}