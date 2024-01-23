/** \file GLFW_OpenGL_GC.cpp */

#include "ephyra_pch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "platform/GLFW/GLFW_OpenGl_GC.h"
#include "systems/log.h"

namespace Ephyra
{

	void errorCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{

	}

	void Ephyra::GLFW_OpenGL_GC::init()
	{
		glfwMakeContextCurrent(m_window);
		auto result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		if (!result) { Log::error("Could not create OpenGL Context for Current GLFW Window: {0}", result); }

		// Enable OpenGl Debug
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(
			[](
				GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam
			)	
			{
				switch (severity)
				{
				case GL_DEBUG_SEVERITY_HIGH :
					Log::error(message);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					Log::warn(message);
					break;
				case GL_DEBUG_SEVERITY_LOW:
					Log::debug(message);
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					Log::info(message);
					break;
				}
			}
		, nullptr);

	}

	void Ephyra::GLFW_OpenGL_GC::swapBuffers()
	{
		glfwSwapBuffers(m_window);
	}

}