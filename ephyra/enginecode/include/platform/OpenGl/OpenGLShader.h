/** \file OpenGLShader.h */
#pragma once

#include "rendering/shader.h"

namespace Ephyra
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const char* vertexFilepath, const char* fragmentFilepath);
		OpenGLShader(const char* filepath);
		virtual ~OpenGLShader();
		virtual uint32_t getID() override { return m_OpenGL_ID; }

		virtual void uploadInt(const char* name, int value) override;
		virtual void uploadIntArray(const char* name, int32_t* value, uint32_t count) override;
		virtual void uploadFloat(const char* name, float value) override;
		virtual void uploadFloat2(const char* name, const glm::vec2& value) override;
		virtual void uploadFloat3(const char* name, const glm::vec3& value) override;
		virtual void uploadFloat3Array(const char* name, glm::vec3* value, uint32_t count) override;
		virtual void uploadFloat4(const char* name, const glm::vec4& value) override;
		virtual void uploadMat4(const char* name, const glm::mat4& value) override;
		
		virtual void useShader(uint32_t modelID) override; //!< Calls Use Shader
		virtual void drawObj(std::shared_ptr<VertexArray> modelVAO) override;
		virtual void drawQuads(std::shared_ptr<VertexArray> modelVAO, uint32_t drawCount) override;

	private:
		uint32_t m_OpenGL_ID;
		void compileAndLink(const char* vertexShaderSrc, const char* fragmentShaderSrc);

	};
}
