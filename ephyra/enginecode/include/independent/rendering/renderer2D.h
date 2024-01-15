/** \file renderer2D.h */
#pragma once

#include <glm/glm.hpp>

#include "rendererCommon.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "systems/log.h"

namespace Engine
{
	class Renderer2DVertex
	{
	public:
		glm::vec4 m_pos;
		glm::vec2 m_uv;
		uint32_t m_texUnit;
		uint32_t m_tint;

		Renderer2DVertex() : m_pos(glm::vec4(0.f)), m_uv({ 0.f }) {}
		Renderer2DVertex(const glm::vec4& pos, const glm::vec2& uv, const uint32_t& texUnit, const glm::vec4& pTint) : m_pos(pos), m_uv(uv), m_texUnit(texUnit), m_tint(RendererCommon::pack(pTint)){}
		static vertexBufferLayout getLayout() { return s_layout; };

	private:
		static vertexBufferLayout s_layout;
	};

	class Quad
	{
	public:
		Quad() = default;
		static Quad createCentreHalfExtents(const glm::vec2& centre, const glm::vec2& halfExtents);
	private:
		glm::vec3 m_translate = glm::vec3(1.0f); //!< Translation Vector
		glm::vec3 m_scale = glm::vec3(1.0f); //!< Scale Vector
		friend class Renderer2D;
	};

	/** \class Renderer2d
	** \brief Class to allow rendering of 2D Primatives
	*/
	class Renderer2D
	{
	public:
		static void init(); //!< Init Internal Data
		static void begin(const SceneWideUniforms& swu); //!< Begin Scene
		static void submit(const Quad& quad, const glm::vec4& tint); //!< Render tinted quad
		static void submit(const Quad& quad, const SubTexture& texture); //!< Render textured quad
		static void submit(const Quad& quad, const SubTexture& texture, const glm::vec4& tint); //!< Render textured and tint quad
		
		static void submit(char ch, const glm::vec2& position, const glm::vec4& tint); // Render A Character
		static void submit(const char* str, const glm::vec2& position, const glm::vec4& tint); // Render A String

		static void submit(char ch, const glm::vec2& position, float &advance, const glm::vec4& tint, uint32_t charSize); // Render A Character
		static void submit(const char* str, const glm::vec2& position, const glm::vec4& tint, uint32_t charSize); // Render A String



		static void end();
		static void flush();
	private:
		struct GlyphData
		{
			glm::vec2 size;
			glm::vec2 bearing;
			float advance;
			SubTexture subTexture;
		};

		struct InternalData
		{
			glm::vec4 defaultTint;
			std::shared_ptr<Shader> shader;
			std::shared_ptr<VertexArray> VAO;
			std::shared_ptr<UniformBuffer> UBO;
			std::array<glm::vec4, 4> quad;
			static const uint32_t batchSize = 8192;
			uint32_t drawCount;
			glm::mat4 model;
			std::vector<Renderer2DVertex> vertices;

			FT_Library ft;
			FT_Face fontFace;
			int32_t charSize;
			unsigned char firstGlyph = 32;
			unsigned char lastGlyph = 126;
			std::vector<GlyphData> glyphData;
			TextureAtlas glyphAtlas;
		};

		static void R2RGBA(unsigned char* dstBuffer, unsigned char* srcBuffer, uint32_t width, uint32_t height);

		static std::shared_ptr<InternalData> s_data;
	};
}
