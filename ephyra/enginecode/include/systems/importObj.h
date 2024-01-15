/** \file importObj.h */
#pragma once

#include "systems/importObj.h"
#include <vector>
#include <rendering/renderer3D.h>

namespace Engine
{
	/** \class importObj
	*	Import class for .obj files
	*/
	class ImportObj
	{
	public:
		ImportObj(const char* filepath);
		~ImportObj() = default;
		inline std::vector<glm::vec3> getPos() { return m_pos; };
		inline std::vector<glm::vec3> getNormal() { return m_normal; };
		inline std::vector<glm::vec2> getUV() { return m_UVs; };
		inline std::vector<Renderer3DVertex> getVertices() { return m_vertices; };
		inline std::vector<uint32_t> getIndices() { return m_indices; };


		std::vector<glm::vec3> m_pos;
		std::vector<glm::vec3> m_normal;
		std::vector<glm::vec2> m_UVs;

		std::vector<Renderer3DVertex> m_vertices;
		std::vector<uint32_t> m_indices;

	};

}
