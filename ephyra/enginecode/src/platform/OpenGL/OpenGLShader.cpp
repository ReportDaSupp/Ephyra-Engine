/** \file OpenGLShader.cpp */

#include "Ephyra_pch.h"

#include "Platform/OpenGl/OpenGLShader.h"
#include <glad/glad.h>
#include <fstream>
#include "Core/Systems/Utility/Log.h"
#include <string>
#include <array>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	OpenGLShader::OpenGLShader(const char* vertexFilepath, const char* fragmentFilepath)
	{
		std::string line, vertexSrc, fragmentSrc;

		std::fstream handle(vertexFilepath, std::ios::in);
		if (handle.is_open())
		{
			while (getline(handle, line)) { vertexSrc += (line + "\n"); }
		}
		else
		{
			Log::error("Could not open shader vertex source: {0}", vertexFilepath);
			return;
		}

		handle.close();

		handle.open(fragmentFilepath, std::ios::in);
		if (handle.is_open())
		{
			while (getline(handle, line)) { fragmentSrc += (line + "\n"); }
		}
		else
		{
			Log::error("Could not open shader fragment source: {0}", fragmentFilepath);
			return;
		}
		handle.close();

		compileAndLink(vertexSrc.c_str(), fragmentSrc.c_str());

	}

	OpenGLShader::OpenGLShader(const char* filepath)
	{
		enum Region { None = -1, Vertex = 0, Fragment, Geometry, TesselationControl, TesselationEvaluation, Compute };
		bool v = false; bool f = false; bool g = false; bool tc = false; bool te = false; bool c = false;
		std::string line;
		std::array<std::string, Region::Compute + 1> src;
		uint32_t region = Region::None;

		std::fstream handle(filepath, std::ios::in);

		if (handle.is_open())
		{

			while (getline(handle, line))
			{
				if (line.find("#region Vertex") != std::string::npos) { region = Region::Vertex; v = true; continue; };
				if (line.find("#region Fragment") != std::string::npos) { region = Region::Fragment; f = true; continue; };
				if (line.find("#region Geometry") != std::string::npos) { region = Region::Geometry; g = true; continue; };
				if (line.find("#region TesselationControl") != std::string::npos) { region = Region::TesselationControl; tc = true; continue; };
				if (line.find("#region TesselationEvaluation") != std::string::npos) { region = Region::TesselationEvaluation; te = true; continue; };
				if (line.find("#region Compute") != std::string::npos) { region = Region::Compute; c = true; continue; };
				if (region != Region::None) src[region] += (line + "\n");
			}

		}
		else
		{
			return;
		}
		handle.close();

		const char* vShaderCode = src[Region::Vertex].c_str();
		const char* gShaderCode = src[Region::Geometry].c_str();
		const char* fShaderCode = src[Region::Fragment].c_str();
		const char* tcShaderCode = src[Region::TesselationControl].c_str();
		const char* teShaderCode = src[Region::TesselationEvaluation].c_str();
		const char* cShaderCode = src[Region::Compute].c_str();

		// 2. compile shaders
		unsigned int vertex, fragment, geometry, tesselationControl, tesselationEvaluation, compute;
		if (v)
		{
			// vertex shader
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			checkCompileErrors(vertex, "VERTEX");
		}
		if (f)
		{
			// fragment Shader
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			checkCompileErrors(fragment, "FRAGMENT");
		}
		if (tc)
		{
			// tesselation control Shader
			tesselationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
			glShaderSource(tesselationControl, 1, &tcShaderCode, NULL);
			glCompileShader(tesselationControl);
			checkCompileErrors(tesselationControl, "TESSELATION_CONTROL");
		}
		if (te)
		{
			// tesselation evaluation Shader
			tesselationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
			glShaderSource(tesselationEvaluation, 1, &teShaderCode, NULL);
			glCompileShader(tesselationEvaluation);
			checkCompileErrors(tesselationEvaluation, "TESSELATION_EVALUATION");
		}
		if (g)
		{
			// geometry Shader
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		if (c)
		{
			// compute Shader
			compute = glCreateShader(GL_COMPUTE_SHADER);
			glShaderSource(compute, 1, &cShaderCode, NULL);
			glCompileShader(compute);
			checkCompileErrors(compute, "COMPUTE");
		}


		// shader Program
		m_OpenGL_ID = glCreateProgram();
		if (v) glAttachShader(m_OpenGL_ID, vertex);
		if (tc) glAttachShader(m_OpenGL_ID, tesselationControl);
		if (te) glAttachShader(m_OpenGL_ID, tesselationEvaluation);
		if (g) glAttachShader(m_OpenGL_ID, geometry);
		if (f) glAttachShader(m_OpenGL_ID, fragment);
		if (c) glAttachShader(m_OpenGL_ID, compute);

		glLinkProgram(m_OpenGL_ID);
		checkCompileErrors(m_OpenGL_ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		if (v) glDeleteShader(vertex);
		if (tc) glDeleteShader(tesselationControl);
		if (te) glDeleteShader(tesselationEvaluation);
		if (g) glDeleteShader(geometry);
		if (f) glDeleteShader(fragment);
		if (c) glDeleteShader(compute);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteShader(m_OpenGL_ID);
	}

	void OpenGLShader::uploadInt(const char* name, int value)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadIntArray(const char* name, int32_t* value, uint32_t count)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform1iv(location, count, value);
	}

	void OpenGLShader::uploadMat4Array(const char* name, glm::mat4* value, uint32_t count)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)value);
	}

	void OpenGLShader::uploadFloat3Array(const char* name, glm::vec3* value, uint32_t count)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform3fv(location, count, (GLfloat*)value);
	}

	void OpenGLShader::uploadFloat(const char* name, float value)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadFloat2(const char* name, const glm::vec2& value)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::uploadFloat3(const char* name, const glm::vec3& value)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::uploadFloat4(const char* name, const glm::vec4& value)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::uploadMat4(const char* name, const glm::mat4& value)
	{
		uint32_t location = glGetUniformLocation(m_OpenGL_ID, name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::useShader(uint32_t modelID)
	{
		glUseProgram(m_OpenGL_ID);
		glBindVertexArray(modelID);
	}

	void OpenGLShader::drawObj(std::shared_ptr<VertexArray> modelVAO)
	{
		glDrawElements(GL_TRIANGLES, modelVAO->getDrawCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLShader::drawQuads(std::shared_ptr<VertexArray> modelVAO, uint32_t drawCount)
	{
		glDrawElements(GL_QUADS, drawCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLShader::compileAndLink(const char* vertexShaderSrc, const char* fragmentShaderSrc)
	{
		// Vertex Shader

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* source = vertexShaderSrc;
		glShaderSource(vertexShader, 1, &source, 0);
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
			{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
			Log::error("Shader compile error: {0}", std::string(infoLog.begin(), infoLog.end()));
			glDeleteShader(vertexShader);
			return;
		};

		// Fragment Shader

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = fragmentShaderSrc;
		glShaderSource(fragmentShader, 1, &source, 0);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
			Log::error("Shader compile error: {0}", std::string(infoLog.begin(), infoLog.end()));

			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			return;
		};

		m_OpenGL_ID = glCreateProgram();
		glAttachShader(m_OpenGL_ID, vertexShader);
		glAttachShader(m_OpenGL_ID, fragmentShader); glLinkProgram(m_OpenGL_ID);

		GLint isLinked = 0;
		glGetProgramiv(m_OpenGL_ID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
			{
			GLint maxLength = 0;
			glGetProgramiv(m_OpenGL_ID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_OpenGL_ID, maxLength, &maxLength, &infoLog[0]);
			Log::error("Shader linking error: {0}", std::string(infoLog.begin(), infoLog.end()));
			glDeleteProgram(m_OpenGL_ID);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			return;
		}

		glDetachShader(m_OpenGL_ID, vertexShader);
		glDetachShader(m_OpenGL_ID, fragmentShader);
	}

	void OpenGLShader::checkCompileErrors(uint32_t shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

}