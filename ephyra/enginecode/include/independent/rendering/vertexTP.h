/** \file vertexTP.h */
#pragma once

#include <glm/glm.hpp>
#include "bufferLayout.h"

namespace Engine
{
	class TPVertex
	{
	public:
		glm::vec3 m_pos;
		glm::vec3 m_normals;
		glm::vec2 m_uvs;
		TPVertex() : m_pos(glm::vec3(0.f)), m_normals(glm::vec3(0.f)), m_uvs(glm::vec2(0.f)) {};
		TPVertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uvs) : m_pos(glm::vec3(pos)), m_normals(glm::vec3(norm)), m_uvs(glm::vec2(uvs)) {};
		static vertexBufferLayout getLayout() { return s_layout; };

	private:
		static vertexBufferLayout s_layout;
	};

	class TPVertexNormalised
	{
	public:
		glm::vec3 m_pos;
		std::array<int16_t, 3> m_normal;
		std::array<int16_t, 2> m_uv;

		TPVertexNormalised() : m_pos(glm::vec3(0.f)), m_normal({ 0,0,0 }), m_uv({ 0, 0 }) {}
		TPVertexNormalised(const glm::vec3& pos, const std::array<int16_t, 3>& normal, const std::array<int16_t, 2> uv) : m_pos(pos), m_normal(normal), m_uv(uv) {}
		static vertexBufferLayout getLayout() { return s_layout; };

	private:
		static vertexBufferLayout s_layout;

	};

	vertexBufferLayout TPVertex::s_layout = { ShaderDataType::Float3, ShaderDataType::Float3, ShaderDataType::Float2 };
	vertexBufferLayout TPVertexNormalised::s_layout = { { ShaderDataType::Float3, {ShaderDataType::Short3, true}, {ShaderDataType::Short2, true} }, 24 };
}