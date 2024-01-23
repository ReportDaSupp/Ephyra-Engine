/** \file GLFWWindowImp.cpp */

#include "ephyra_pch.h"
#include "platform/GLFW/GLFWWindowImp.h"
#include "platform/GLFW/GLFW_OpenGl_GC.h"
#include "systems/log.h"
#include <core/globalProperties.h>

namespace Ephyra
{

#ifdef NG_PLATFORM_WINDOWS

	Window* Window::create(const WindowProperties& properties)
	{
		return new GLFWWindowImp(properties);
	}

#endif

	GLFWWindowImp::GLFWWindowImp(const WindowProperties& properties)
	{
		init(properties);
	}

	void GLFWWindowImp::init(const WindowProperties& properties)
	{
		m_props = properties;

		GLFWmonitor* primary = glfwGetPrimaryMonitor();

		const GLFWvidmode* mode = glfwGetVideoMode(primary);

		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		m_props.m_width = SCR_WIDTH;
		m_props.m_height = SCR_HEIGHT;

		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

		m_aspectRatio = static_cast<float>(ASPECT_RATIO.x / ASPECT_RATIO.y);

		m_native = glfwCreateWindow(m_props.m_width, m_props.m_height, m_props.m_title, nullptr, nullptr);
		glfwSetWindowPos(m_native, 50, 50);

		m_graphicsContext.reset(new GLFW_OpenGL_GC(m_native));
		m_graphicsContext->init();

		glfwSetWindowUserPointer(m_native, static_cast<void*>(&m_eventHandler));

		// GLFW Window Callbacks

		glfwSetWindowCloseCallback(m_native,
			[](GLFWwindow* window)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));
				auto& onClose= handler->getOnCloseCallback();

				WindowCloseEvent e;
				onClose(e);
			}
		);

		glfwSetWindowSizeCallback(m_native, 
			[](GLFWwindow* window, int newWidth, int newHeight)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));
				auto& onResize = handler->getOnResizeCallback();

				WindowResizeEvent e(newWidth, newHeight);
				onResize(e);
			}
		);

		glfwSetWindowPosCallback(m_native,
			[](GLFWwindow* window, int newXPos, int newYPos)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));
				auto& onMoved = handler->getOnMovedCallback();

				WindowMovedEvent e(newXPos, newYPos);
				onMoved(e);
			}
		);

		glfwSetWindowFocusCallback(m_native,
			[](GLFWwindow* window, int focus)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));

				if (focus == GLFW_TRUE) { 
					auto& onFocus = handler->getOnFocusCallback();
					WindowFocusEvent e; 
					onFocus(e); 
				}
				else { 
					auto& onLostFocus = handler->getOnLostFocusCallback();
					WindowLostFocusEvent e; 
					onLostFocus(e);
				}
			}
		);

		// GLFW Key Callbacks

		glfwSetKeyCallback(m_native,
			[](GLFWwindow* window, int keyCode, int scanCode, int action, int mods)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));
				
				if (action == GLFW_PRESS)
				{
					auto& onKeyPress = handler->getOnKeyPressedCallback();

					KeyPressedEvent e(keyCode, 0);
					onKeyPress(e);
				}
				else if (action == GLFW_REPEAT)
				{
					auto& onKeyPress = handler->getOnKeyPressedCallback();

					KeyPressedEvent e(keyCode, 1);
					onKeyPress(e);
				}
				else if (action == GLFW_RELEASE)
				{
					auto& onKeyReleased = handler->getOnKeyReleasedCallback();

					KeyReleasedEvent e(keyCode);
					onKeyReleased(e);
				}
			}
			);

		// GLFW Mouse Callbacks

		glfwSetMouseButtonCallback(m_native,
			[](GLFWwindow* window, int button, int action, int mods)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));

				if (action == GLFW_PRESS)
				{
					auto& onMouseDown = handler->getOnMouseDownCallback();

					MouseButtonPressedEvent e(button);
					onMouseDown(e);
				}
				else if (action == GLFW_RELEASE)
				{
					auto& onMouseUp = handler->getOnMouseUpCallback();

					MouseButtonReleasedEvent e(button);
					onMouseUp(e);
				}

			}
		);

		glfwSetCursorPosCallback(m_native,
			[](GLFWwindow* window, double newXPos, double newYPos)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));
				auto& onMouseMoved = handler->getOnMouseMovedCallback();

				MouseMovedEvent e(newXPos, newYPos);
				onMouseMoved(e);
			}
		);

		glfwSetScrollCallback(m_native,
			[](GLFWwindow* window, double newXOffset, double newYOffset)
			{
				EventHandler* handler = static_cast<EventHandler*>(glfwGetWindowUserPointer(window));
				auto& onMouseScrolled = handler->getOnMouseWheelCallback();

				MouseScrolledEvent e(newXOffset, newYOffset);
				onMouseScrolled(e);
			}
		);

	}

	void GLFWWindowImp::close()
	{
		glfwDestroyWindow(m_native);
	}

	void GLFWWindowImp::onUpdate(float timestep)
	{
		glfwPollEvents();
		m_graphicsContext->swapBuffers();
	}

	void GLFWWindowImp::setVSync(bool VSync)
	{
		m_props.m_isVSync = VSync;
		if (VSync) { glfwSwapInterval(1); }
		else { glfwSwapInterval(0); }
	}
}