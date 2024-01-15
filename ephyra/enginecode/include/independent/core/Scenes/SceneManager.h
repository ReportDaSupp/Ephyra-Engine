/**
 * \file SceneManager.h
 * \brief Defines the SceneManager class.
 */

#pragma once
#define GLFW_INCLUDE_NONE
#include <string>
#include <unordered_map>
#include <engine.h>

#include "core/Scenes/BaseScene.h"
#include "core/Scenes/IntroScene.h"

namespace Engine
{

	/**
	* \class SceneManager
	* \brief Manages the scenes in the game.
	*/
	class SceneManager
	{
	public:
		/**
		* \brief Constructs a SceneManager object.
		*/
		SceneManager();

		/**
		* \brief Destructs a SceneManager object.
		*/
		~SceneManager();

		/**
		* \brief Loads a scene.
		* \param sceneName The name of the scene.
		* \param scene The scene to be loaded.
		*/
		void LoadScene(const std::string& sceneName, std::shared_ptr<BaseScene> scene);

		/**
		* \brief Unloads a scene.
		* \param sceneName The name of the scene.
		*/
		void UnloadScene(const std::string& sceneName);

		/**
		* \brief Sets the active scene.
		* \param sceneName The name of the scene.
		*/
		void SetActiveScene(const std::string& sceneName);

		/**
		* \brief Updates the active scene.
		* \param deltaTime The time since the last frame.
		* \return True if the scene was updated successfully, false otherwise.
		*/
		bool UpdateActiveScene(float deltaTime);

		/**
		* \brief Sets the game to fullscreen mode.
		*/
		void setFullscreen();

		/**
		* \brief Attaches a camera to the scene.
		* \param cameraName The name of the camera.
		*/
		void AttachCamera(const std::string& cameraName);

		/**
		* \brief Gets the window.
		* \return The window.
		*/
		std::shared_ptr<Window> getWindow() { return m_window; };

		/**
		* \brief Gets the input poller.
		* \return The input poller.
		*/
		std::shared_ptr<InputPoller> getPoller() { return m_poller; };

	protected:
		std::unordered_map<std::string, std::shared_ptr<BaseScene>> scenes; /**< The scenes in the game. */
		std::shared_ptr<BaseScene> activeScene; /**< The active scene. */

		std::shared_ptr<Log> m_logSystem; /**< The log system. */
		std::shared_ptr<RandomNumGen> m_randomNumSystem; /**< The random number system. */
		std::shared_ptr<System> m_windowsSystem; /**< The windows system. */

		std::shared_ptr<Window> m_window; /**< The window. */
		std::shared_ptr<Timer> m_timer; /**< The timer. */

		std::shared_ptr<InputPoller> m_poller; /**< The input poller. */

		bool m_running = true; /**< Flag indicating if the application is running. */
		bool m_focus = true; /**< Flag indicating if the application is focused. */
		bool m_fullScreen = false; /**< Flag indicating if the game is in fullscreen mode. */

		EventHandler m_handler; /**< The event handler. */

		// Window Event Handling

		/**
		* \brief Handles the window close event.
		* \param e The window close event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onClose(WindowCloseEvent& e)
		{
			e.handle(true);
			m_running = false;
			return e.handled();
		}

		/**
		* \brief Handles the window resize event.
		* \param e The window resize event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onResize(WindowResizeEvent& e)
		{
			e.handle(true);
			auto& size = e.getSize();
			Log::info("Window Resize event: ({0}, {1})", size.x, size.y);
			return e.handled();
		}

		/**
		* \brief Handles the window moved event.
		* \param e The window moved event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onMoved(WindowMovedEvent& e)
		{
			e.handle(true);
			auto& position = e.getPos();
			Log::info("Window Moved event: ({0}, {1})", position.x, position.y);
			return e.handled();
		}

		/**
		* \brief Handles the window focus event.
		* \param e The window focus event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onFocus(WindowFocusEvent& e)
		{
			e.handle(true);
			m_focus = true;
			return e.handled();
		}

		/**
		* \brief Handles the window lost focus event.
		* \param e The window lost focus event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onLostFocus(WindowLostFocusEvent& e)
		{
			e.handle(true);
			m_focus = false;
			return e.handled();
		}

		// Key Event Handling

		/**
		* \brief Handles the key pressed event.
		* \param e The key pressed event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onKeyPressed(KeyPressedEvent& e)
		{
			e.handle(true);
			int keycode = e.getKeyCode();
			int scancode = glfwGetKeyScancode(keycode);
			int action = 1;
			int mod = 0;
			const char* key = glfwGetKeyName(keycode, scancode);
			Log::info("Key Pressed event - Key: {0}", key);
			if (0 <= key < 350)
				m_poller->processKeyPress(keycode, scancode, action, mod);
			return e.handled();
		}

		/**
		* \brief Handles the key released event.
		* \param e The key released event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onKeyReleased(KeyReleasedEvent& e)
		{
			e.handle(true);
			int keycode = e.getKeyCode();
			int scancode = glfwGetKeyScancode(keycode);
			int action = 0;
			int mod = 0;
			const char* key = glfwGetKeyName(keycode, scancode);
			Log::info("Key Released event - Key: {0}", key);
			if (0 <= key < 350)
				m_poller->processKeyPress(keycode, scancode, action, mod);
			return e.handled();
		}

		// Mouse Event Handling

		/**
		* \brief Handles the mouse moved event.
		* \param e The mouse moved event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onMouseMoved(MouseMovedEvent& e)
		{
			e.handle(true);
			auto& position = e.getPos();
			Log::info("Mouse Moved event: ({0}, {1})", position.x, position.y);
			m_poller->processMouseMove(position.x, position.y);
			return e.handled();
		}

		/**
		* \brief Handles the mouse button pressed event.
		* \param e The mouse button pressed event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onMousePressed(MouseButtonPressedEvent& e)
		{
			e.handle(true);
			auto button = e.getButton();
			Log::info("Mouse Button Pressed event: ({0})", button);
			m_poller->processMouseClick(button, 1, 0);
			return e.handled();
		}

		/**
		* \brief Handles the mouse button released event.
		* \param e The mouse button released event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onMouseReleased(MouseButtonReleasedEvent& e)
		{
			e.handle(true);
			auto button = e.getButton();
			Log::info("Mouse Button Released event: ({0})", button);
			m_poller->processMouseClick(button, 0, 0);
			return e.handled();
		}

		/**
		* \brief Handles the mouse scrolled event.
		* \param e The mouse scrolled event.
		* \return True if the event was handled, false otherwise.
		*/
		bool onMouseWheel(MouseScrolledEvent& e)
		{
			e.handle(true);
			auto xOffset = e.getXOffset();
			auto yOffset = e.getYOffset();
			Log::info("Mouse Moved event: ({0}, {1})", xOffset, yOffset);
			m_poller->processMouseScroll(xOffset, yOffset);
			return e.handled();
		}
	};
}