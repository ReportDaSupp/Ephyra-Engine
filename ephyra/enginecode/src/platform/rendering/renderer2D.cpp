/** \file renderer2D.cpp */

#include "ephyra_pch.h"
#include "rendering/renderer2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include <numeric>
#include <algorithm>

namespace Ephyra
{
	vertexBufferLayout Renderer2DVertex::s_layout = vertexBufferLayout{ ShaderDataType::Float4, ShaderDataType::Float2, ShaderDataType::FlatInt, {ShaderDataType::Byte4, 0, true} };

	std::shared_ptr<Renderer2D::InternalData> Renderer2D::s_data = nullptr;

	void Renderer2D::init()
	{
		s_data.reset(new InternalData);

		if (!RendererCommon::m_textUM)
		{
			RendererCommon::m_textUM.reset(new TextureUnitManager(32));
		}

		if (!RendererCommon::defaultTexture)
		{
			unsigned char whitePx[4] = { 255, 255, 255, 255 };
			RendererCommon::defaultTexture.reset(Texture::create(1, 1, 4, whitePx));

			RendererCommon::defaultSubTexture = SubTexture(RendererCommon::defaultTexture, { 0.f,0.f }, { 1.f, 1.f });
		}

		if (!RendererCommon::textureUnits)
		{
			RendererCommon::textureUnits.reset(new std::array<int32_t, 32>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 });
		}

		if (RendererCommon::defaultTint)
		{
			RendererCommon::defaultTint.reset(new glm::vec4({ 1.f, 1.f,1.f,1.f }));
		}

		s_data->defaultTint = {1.f, 1.f, 1.f, 1.f};
		s_data->model = glm::mat4(1.f);

		s_data->shader.reset(Shader::create("./assets/shaders/quad1.glsl"));

		s_data->UBO.reset(UniformBuffer::create(uniformBufferLayout{
			{"u_projection", ShaderDataType::Mat4},
			{"u_view", ShaderDataType::Mat4}
			}));

		s_data->quad[0] = {	-0.5f,	-0.5f,	1.f,	1.f	};
		s_data->quad[1] = {	-0.5f,	 0.5f,	1.f,	1.f	};
		s_data->quad[2] = {	 0.5f,	 0.5f,	1.f,	1.f	};
		s_data->quad[3] = {	 0.5f,	-0.5f,	1.f,	1.f	};
		
		s_data->vertices.resize(s_data->batchSize);
		s_data->vertices[0] = Renderer2DVertex(	s_data->quad[0],	{0.0f,  0.0f},	0, glm::vec4(1.f));
		s_data->vertices[1] = Renderer2DVertex(	s_data->quad[1],	{0.0f,	1.0f},	0, glm::vec4(1.f));
		s_data->vertices[2] = Renderer2DVertex(	s_data->quad[2],	{1.0f,	1.0f},	0, glm::vec4(1.f));
		s_data->vertices[3] = Renderer2DVertex(	s_data->quad[3],	{1.0f,	0.0f},	0, glm::vec4(1.f));
		
		std::vector<uint32_t> indices(s_data->batchSize);

		std::iota(indices.begin(), indices.end(), 0);

		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;

		s_data->VAO.reset(VertexArray::create());
		VBO.reset(VertexBuffer::create(s_data->vertices.data(), sizeof(Renderer2DVertex) * s_data->vertices.size(), Renderer2DVertex::getLayout()));
		IBO.reset(IndexBuffer::create(indices.data(), indices.size()));

		s_data->VAO->addVertexBuffer(VBO);
		s_data->VAO->setIndexBuffer(IBO);

		s_data->UBO->attachShaderBlock(s_data->shader, "b_camera");

		// Filepath to Font
		const char* filepath = ("./assets/fonts/BOOKOSB.TTF");

		// Init FreeType
		if (FT_Init_FreeType(&s_data->ft)) 
			Log::error("Error: FreeType Could Not Be Initialized!");

		// Load The Font
		if (FT_New_Face(s_data->ft, filepath, 0, &s_data->fontFace)) 
			Log::error("Error: Font Could Not Be Initialized!");

		// Set Font Size
		s_data->charSize = 25;
		if (FT_Set_Pixel_Sizes(s_data->fontFace, 0, s_data->charSize))
			Log::error("Error: FreeType Can't Set Font Size: {0}", s_data->charSize);

		// Fill The Texture Atlas
		s_data->glyphData.resize(s_data->lastGlyph - s_data->firstGlyph + 1);

		std::sort(s_data->glyphData.begin(), s_data->glyphData.end(),
			[](GlyphData& a, GlyphData& b)
			{
				return (a.size.x * a.size.y) > (b.size.x * b.size.y);
			});

		for (unsigned char ch = s_data->firstGlyph; ch <= s_data->lastGlyph; ch++)
		{

			// Get Glyph From FreeType
			if (FT_Load_Char(s_data->fontFace, ch, FT_LOAD_RENDER)) Log::error("Error: Could Not Load Glyph For Char: {0}", ch);
			else
			{
				GlyphData& gd = s_data->glyphData.at(ch - s_data->firstGlyph);
				// Get Glyph Data
				gd.size = glm::vec2(s_data->fontFace->glyph->bitmap.width, s_data->fontFace->glyph->bitmap.rows);
				gd.bearing = glm::vec2(s_data->fontFace->glyph->bitmap_left, -s_data->fontFace->glyph->bitmap_top);

				// Calculate Advance
				gd.advance = static_cast<float>(s_data->fontFace->glyph->advance.x >> 6);

				// Set The Dims of the Glyph Buffer
				glm::vec2 glyphBufferDims = gd.size;
				uint32_t glyphBufferSize = glyphBufferDims.x * glyphBufferDims.y * 4 * sizeof(unsigned char);
				unsigned char* glyphBuffer = static_cast<unsigned char*>(malloc(glyphBufferSize));

				// Sort Glyph Texture
				R2RGBA(glyphBuffer, s_data->fontFace->glyph->bitmap.buffer, gd.size.x, gd.size.y);
				s_data->glyphAtlas.add(gd.size.x, gd.size.y, 4, glyphBuffer, gd.subTexture);

				free(glyphBuffer);
			}
		}
	}

	void Renderer2D::begin(const SceneWideUniforms& swu)
	{
		s_data->drawCount = 0;

		// Bind the Shader
		s_data->shader->useShader(s_data->VAO->getRenderID());

		s_data->shader->uploadIntArray("u_texData", RendererCommon::textureUnits->data(), 32);
		s_data->UBO->bindUniformBuffer();

		//Pack These
		s_data->UBO->uploadData("u_projection", swu.at("u_projection").second);
		s_data->UBO->uploadData("u_view", swu.at("u_view").second);
	}

	void Renderer2D::submit(const Quad& quad, const glm::vec4& tint)
	{
		Renderer2D::submit(quad, RendererCommon::defaultSubTexture, tint);
	}

	void Renderer2D::submit(const Quad& quad, const SubTexture& texture)
	{
		Renderer2D::submit(quad, texture, s_data->defaultTint);
	}

	void Renderer2D::submit(const Quad& quad, const SubTexture& texture, const glm::vec4& tint)
	{
		if (s_data->drawCount + 4 > s_data->batchSize) flush();

		uint32_t texUnit;
		if (RendererCommon::m_textUM->isFull()) {
			flush();
			RendererCommon::m_textUM->clear();
		}

		s_data->shader->useShader(s_data->VAO->getRenderID());

		uint32_t m_unit;
		RendererCommon::m_textUM->getUnit(texture.getTextureID(), m_unit);

		texture.getBaseTexture()->load(*RendererCommon::m_textUM, m_unit);
		s_data->model = glm::scale(glm::translate(glm::mat4(1.0f), quad.m_translate), quad.m_scale);

		RendererCommon::m_textUM->getUnit(texture.getBaseTexture()->getID(), texUnit);

		uint32_t startIdx = s_data->drawCount;
		for (int i = 0; i < 4; i++)
		{
			s_data->vertices[i + startIdx].m_pos = s_data->model * s_data->quad[i];
			s_data->vertices[i + startIdx].m_tint = RendererCommon::pack(tint);
			s_data->vertices[i + startIdx].m_texUnit = texUnit;
		}

		s_data->vertices[startIdx + 0].m_uv = { texture.getUVStart().x, texture.getUVStart().y };
		s_data->vertices[startIdx + 1].m_uv = { texture.getUVStart().x, texture.getUVEnd().y };
		s_data->vertices[startIdx + 2].m_uv = { texture.getUVEnd().x, texture.getUVEnd().y };
		s_data->vertices[startIdx + 3].m_uv = { texture.getUVEnd().x, texture.getUVStart().y };

		s_data->drawCount += 4;
	}

	void Renderer2D::submit(char ch, const glm::vec2& position, const glm::vec4& tint)
	{
		float advance = 0.f;
		submit(ch, position, advance, tint, 100);
	}

	void Renderer2D::submit(const char* str, const glm::vec2& position, const glm::vec4& tint)
	{
		submit(str, position, tint, 100);
	}

	void Renderer2D::submit(char ch, const glm::vec2& position, float &advance, const glm::vec4& tint, uint32_t charSize)
	{
		/*SubTexture tmp(s_data->glyphAtlas.getBaseTexture(), glm::vec2(0.f), glm::vec2(1.f));
		Quad quad;
		quad = Quad::createCentreHalfExtents(glm::vec2(512, 400), glm::vec2(512, 400));
		submit(quad, tmp, {1,1,1,1});*/

		float charScale = (float)charSize / s_data->charSize;

		if (ch >= s_data->firstGlyph && ch <= s_data->lastGlyph)
		{
			GlyphData& gd = s_data->glyphData.at(ch - s_data->firstGlyph);

			advance = gd.advance * (charScale);
			// Calculate the Quad for Glyph
			glm::vec2 glyphHalfExtents((gd.size * glm::vec2((charScale), (charScale)) * 0.5f));
			glm::vec2 glyphCentre = (position + gd.bearing * glm::vec2((charScale), (charScale)) + glyphHalfExtents);
			Quad quad = Quad::createCentreHalfExtents(glyphCentre, glyphHalfExtents);

			submit(quad, gd.subTexture, tint);

		}
	}

	void Renderer2D::submit(const char* str, const glm::vec2& position, const glm::vec4& tint, uint32_t charSize)
	{
		uint32_t len = strlen(str);
		float l_Advance = 0.f, x = position.x;

		for (int32_t i = 0; i < len; i++)
		{
			submit(str[i], { x, position.y }, l_Advance, tint, charSize);
			x += l_Advance;
		}
	}

	void Renderer2D::end()
	{
		if (s_data->drawCount > 0) flush();
	}

	void Renderer2D::flush()
	{
		s_data->VAO->getVertexBuffer().at(0)->edit(s_data->vertices.data(), sizeof(Renderer2DVertex) * s_data->vertices.size(), 0);

		s_data->VAO->bindIndexBuffer();
		s_data->shader->drawQuads(s_data->VAO, s_data->drawCount);

		s_data->drawCount = 0;
	}

	void Renderer2D::R2RGBA(unsigned char* dstBuffer, unsigned char* srcBuffer, uint32_t width, uint32_t height)
	{
		unsigned char* ptrWalker = dstBuffer;
		for (int32_t i = 0; i < height; i++)
		{
			for (int32_t j = 0; j < width; j++)
			{
				*ptrWalker = 255; ptrWalker++;
				*ptrWalker = 255; ptrWalker++;
				*ptrWalker = 255; ptrWalker++;
				*ptrWalker = *srcBuffer; //set Alpha Channel
				ptrWalker++;
				*srcBuffer++;
			}
		}
	}

	Quad Quad::createCentreHalfExtents(const glm::vec2& centre, const glm::vec2& halfExtents)
	{
		Quad result;

		result.m_translate = glm::vec3(centre, 0.f);
		result.m_scale = glm::vec3(2.f * halfExtents, 1.f);

		return result;

	}

}