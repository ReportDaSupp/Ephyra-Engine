/** \file GLFW_OpenGL_GC.h */
#pragma once

#include "core/graphicsContext.h"

namespace Engine
{
	class GLFW_OpenGL_GC : public GraphicsContext
	{
	public:
		GLFW_OpenGL_GC(GLFWwindow* win) : m_window(win) {}
		virtual void init() override; //!< Init the graphics context for window api
		virtual void swapBuffers() override; //!< Swap front and back buffers (Double Buffering)
	private:
		GLFWwindow* m_window; //!< Pointer to GLFW window
	};
}
