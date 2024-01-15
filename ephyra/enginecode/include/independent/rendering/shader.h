/** \file shader.h */
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "rendering/vertexArray.h"

namespace Engine
{

	/** \class Shader
	*	API Agnostic Shader Class
	*/

	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual uint32_t getID() = 0;

		virtual void uploadInt(const char* name, int value) = 0;
		virtual void uploadIntArray(const char* name, int32_t* value, uint32_t count) = 0;
		virtual void uploadFloat(const char* name, float value) = 0;
		virtual void uploadFloat2(const char* name, const glm::vec2& value) = 0;
		virtual void uploadFloat3(const char* name, const glm::vec3& value) = 0;
		virtual void uploadFloat3Array(const char* name, glm::vec3* value, uint32_t count) = 0;
		virtual void uploadFloat4(const char* name, const glm::vec4& value) = 0;
		virtual void uploadMat4(const char* name, const glm::mat4& value) = 0;

		virtual void useShader(uint32_t modelID) = 0; //!< Calls Use Shader
		virtual void drawObj(std::shared_ptr<VertexArray> modelVAO) = 0;
		virtual void drawQuads(std::shared_ptr<VertexArray> modelVAO, uint32_t drawCount) = 0;

		static Shader* create(const char* vertexFilepath, const char* fragmentFilepath);
		static Shader* create(const char* filepath);
	};
}