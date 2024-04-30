/** \file application.cpp */

#include "Ephyra_pch.h"
#include "Core/Initialization/Application.h"

namespace Engine 
{

		// Set static variables
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		if (s_instance == nullptr)
		{
			s_instance = this;
		}

		// Init Variables
		m_running = true;

		// Start log
		m_logSystem.reset(new Log);
		m_logSystem->start();

		// reset timer
		m_timer.reset(new ChronoTimer);
		m_timer->start();

		// reset windows system
		m_windowsSystem.reset(new GLFWSystem);

		// Start the windows system
		m_windowsSystem->start();

		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);

		// Define Window
		WindowProperties props("Ephyra Animation Engine - Untitled.eph", SCR_WIDTH, SCR_HEIGHT, false); //!< Window Properties

		m_window.reset(Window::create(props));

		SCR_WIDTH = m_window->getWidthf();
		SCR_HEIGHT = m_window->getHeightf();

		// Start input poller
		m_poller.reset(new InputPoller);

		// Window Callbacks For Event handling

		m_window->getEventHandler().setOnCloseCallback(std::bind(&Application::onClose, this, std::placeholders::_1));
		m_window->getEventHandler().setOnResizeCallback(std::bind(&Application::onResize, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMovedCallback(std::bind(&Application::onMoved, this, std::placeholders::_1));
		m_window->getEventHandler().setOnFocusCallback(std::bind(&Application::onFocus, this, std::placeholders::_1));
		m_window->getEventHandler().setOnLostFocusCallback(std::bind(&Application::onLostFocus, this, std::placeholders::_1));

		// Keyboard Callbacks For Event handling

		m_window->getEventHandler().setOnKeyPressedCallback(std::bind(&Application::onKeyPressed, this, std::placeholders::_1));
		m_window->getEventHandler().setOnKeyReleasedCallback(std::bind(&Application::onKeyReleased, this, std::placeholders::_1));

		// Mouse Callbacks For Event handling

		m_window->getEventHandler().setOnMouseMovedCallback(std::bind(&Application::onMouseMoved, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMouseDownCallback(std::bind(&Application::onMousePressed, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMouseUpCallback(std::bind(&Application::onMouseReleased, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMouseWheelCallback(std::bind(&Application::onMouseWheel, this, std::placeholders::_1));

		glfwSetInputMode((GLFWwindow*)m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		m_timer->reset();

			// Layer Stack
		m_layerStack.reset(new LayerStack(m_window, m_poller));

	}

	Application::~Application()
	{
		m_windowsSystem->stop();
		m_logSystem->stop();

	}

	void Application::run()
	{
		while (m_running)
		{
			deltaTime = m_timer->getElapsedTime();
			m_timer->reset();
			for (auto layer = m_layerStack->begin(); layer != m_layerStack->end(); layer++)
				(*layer)->OnUpdate(deltaTime);

			m_window->onUpdate(deltaTime);


		}
	}

}
