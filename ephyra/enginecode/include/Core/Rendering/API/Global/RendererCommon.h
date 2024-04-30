/**\file rendererCommon.h*/
#pragma once
#include <glm/glm.hpp>
#include <unordered_map>

#include "Core/Rendering/API/Buffers/FrameBuffer.h"
#include "Core/Rendering/API/Buffers/uniformBuffer.h"
#include "Core/Rendering/API/Global/RenderCommands.h"
#include "Core/Rendering/API/Global/RenderAPI.h"
#include "Core/Rendering/API/Shaders/ShaderDataType.h"
#include "Core/Rendering/API/Shaders/Shader.h"
#include "Core/Rendering/API/Textures/Texture.h"
#include "Core/Rendering/API/Textures/SubTexture.h"
#include "Core/Rendering/API/Utility/PostProcessing.h"
#include "Core/Resources/Utility/TextureAtlas.h"
#include "Core/Resources/Utility/TextureUnitManager.h"

namespace Engine
{
	using SceneWideUniforms = std::unordered_map<const char*, std::pair<ShaderDataType, void*>>;

	class RendererCommon
	{
	public:
		static void actionCommand(std::shared_ptr<RenderCommand>& command) { command->m_action(); }
		inline static std::shared_ptr<TextureUnitManager> m_textUM;
		inline static std::shared_ptr<Texture> defaultTexture;
		inline static std::shared_ptr<Texture> defaultNormalTexture;
		inline static SubTexture defaultSubTexture;
		inline static std::shared_ptr<glm::vec4> defaultTint;
		inline static std::shared_ptr <std::array<int32_t, 32>> textureUnits;
		inline static FrameBuffer* colorFBO;
		inline static std::shared_ptr<Texture> colorFBOTexture;
		inline static std::shared_ptr<Texture> depthFBOTexture;
		inline static PostProcessing* postProcessor;

		inline static float frameCount;

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