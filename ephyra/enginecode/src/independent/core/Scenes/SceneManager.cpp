#include "ephyra_pch.h"

#include "core/Scenes/SceneManager.h"
#include "systems/perlinNoise.h"

namespace Ephyra {

	SceneManager::SceneManager()
	{
			// Start Continuous Window Systems

		// Start log
		m_logSystem.reset(new Log);
		m_logSystem->start();

		// Start random number system
		m_randomNumSystem.reset(new RandomNumGen);
		m_randomNumSystem->start();

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
		WindowProperties props("IMAT3904 Game Engine", SCR_WIDTH, SCR_HEIGHT, false); //!< Window Properties

		m_window.reset(Window::create(props));

		SCR_WIDTH = m_window->getWidthf();
		SCR_HEIGHT = m_window->getHeightf();

		// Start input poller
		m_poller.reset(new InputPoller);

			// Window Callbacks For Event handling

		m_window->getEventHandler().setOnCloseCallback(std::bind(&SceneManager::onClose, this, std::placeholders::_1));
		m_window->getEventHandler().setOnResizeCallback(std::bind(&SceneManager::onResize, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMovedCallback(std::bind(&SceneManager::onMoved, this, std::placeholders::_1));
		m_window->getEventHandler().setOnFocusCallback(std::bind(&SceneManager::onFocus, this, std::placeholders::_1));
		m_window->getEventHandler().setOnLostFocusCallback(std::bind(&SceneManager::onLostFocus, this, std::placeholders::_1));

			// Keyboard Callbacks For Event handling

		m_window->getEventHandler().setOnKeyPressedCallback(std::bind(&SceneManager::onKeyPressed, this, std::placeholders::_1));
		m_window->getEventHandler().setOnKeyReleasedCallback(std::bind(&SceneManager::onKeyReleased, this, std::placeholders::_1));

			// Mouse Callbacks For Event handling

		m_window->getEventHandler().setOnMouseMovedCallback(std::bind(&SceneManager::onMouseMoved, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMouseDownCallback(std::bind(&SceneManager::onMousePressed, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMouseUpCallback(std::bind(&SceneManager::onMouseReleased, this, std::placeholders::_1));
		m_window->getEventHandler().setOnMouseWheelCallback(std::bind(&SceneManager::onMouseWheel, this, std::placeholders::_1));

		glfwSetInputMode((GLFWwindow*)m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		m_timer->reset();

		activeScene = nullptr;
	}

	SceneManager::~SceneManager()
	{
			// Stop systems
			
		// Stop Random Number System
		m_randomNumSystem->stop();

		// Stop Logger
		m_logSystem->stop();

		// Stop GLFW Window System
		m_windowsSystem->stop();
	}

	void SceneManager::LoadScene(const std::string& sceneName, std::shared_ptr<BaseScene> scene)
	{
		scenes[sceneName] = scene;
	}

	void SceneManager::UnloadScene(const std::string& sceneName)
	{
		scenes.erase(sceneName);
	}

	void SceneManager::SetActiveScene(const std::string& sceneName)
	{
		activeScene = scenes[sceneName];
	}

	bool SceneManager::UpdateActiveScene(float deltaTime)
	{
		if (activeScene->isFullscreen() != m_fullScreen)
		{
			this->setFullscreen();
		}
		return activeScene->OnUpdate(deltaTime);
	}

	// 1244 / 800 = 1.555
	// x = 1.555 * h


	void SceneManager::setFullscreen()
	{
		GLFWmonitor* primary = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primary);
		if (!m_fullScreen)
		{	
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowPos((GLFWwindow*)(m_window->getNativeWindow()), 0, 0);
			glfwSetWindowSize((GLFWwindow*)(m_window->getNativeWindow()), mode->width, mode->height);
			
		}
		else
		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			glfwSetWindowPos((GLFWwindow*)(m_window->getNativeWindow()), 50, 50);
			glfwSetWindowSize((GLFWwindow*)(m_window->getNativeWindow()), SCR_WIDTH, SCR_HEIGHT);
		}
		m_fullScreen = activeScene->isFullscreen();
	}

	void SceneManager::AttachCamera(const std::string& cameraName)
	{
		activeScene->AttachCamera(cameraName);
	}
}