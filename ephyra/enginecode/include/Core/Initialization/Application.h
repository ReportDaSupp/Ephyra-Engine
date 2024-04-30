#pragma once

#include <functional>
#include <glfw/glfw3.h>
#include <reactphysics3d/reactphysics3d.h>
#include <src/entt/entt.hpp>

#include "Core/Initialization/Window.h"
#include "Core/Resources/Utility/CameraFPS.h"
#include "Core/Resources/Utility/LayerStack.h"
#include "Core/Systems/Utility/Log.h"
#include "Core/Systems/Utility/Timer.h"
#include "Core/Systems/Events/Event.h"
#include "Core/Systems/Events/EventHandler.h"
#include "Core/Systems/Events/InputPoller.h"

#include "Platform/GLFW/GLFWSystem.h"


namespace Engine {

    /**
     * \class Application
     * \brief Fundemental class of the engine. A singleton which runs the game loop infinitely.
     * 
     * The Application class is a fundamental class of the engine. It is a singleton class that runs the game loop infinitely. It provides various functionalities for the engine.
     */
	class Application
	{
	protected:
		Application(); //!< Constructor

		std::shared_ptr<Log> m_logSystem;
		std::shared_ptr<Timer> m_timer;

		std::shared_ptr<System> m_windowsSystem;
		std::shared_ptr<Window> m_window;
		std::shared_ptr<InputPoller> m_poller;

		std::shared_ptr<LayerStack> m_layerStack;

		float deltaTime;
		

		EventHandler m_handler;

		// Window Event Handling

		bool onClose(WindowCloseEvent& e)
		{
			e.handle(true);
			m_running = false;
			return e.handled();
		}
		
		bool onResize(WindowResizeEvent& e)
		{
			e.handle(true);
			auto& size = e.getSize();
			Log::info("Window Resize event: ({0}, {1})", size.x, size.y);
			return e.handled();
		}
		
		bool onMoved(WindowMovedEvent& e)
		{
			e.handle(true);
			auto& position = e.getPos();
			Log::info("Window Moved event: ({0}, {1})", position.x, position.y);
			return e.handled();
		}

		bool onFocus(WindowFocusEvent& e)
		{
			e.handle(true);
			m_focus = true;
			return e.handled();
		}

		bool onLostFocus(WindowLostFocusEvent& e)
		{
			e.handle(true);
			m_focus = false;
			return e.handled();
		}

		// Key Event Handling

		bool onKeyPressed(KeyPressedEvent& e)
		{
			int keycode = e.getKeyCode();
			int scancode = glfwGetKeyScancode(keycode);
			int action = 1;
			int mod = 0;
			const char* key = glfwGetKeyName(keycode, scancode);
			Log::info("Key Pressed event - Key: {0}", key);
			switch (keycode)
			{
			/*case GLFW_KEY_ESCAPE:
				m_running = false;
				e.handle(true);
				return e.handled();
				break;*/
			default:
				return m_layerStack->OnKeyPress(e);
				break;
			}
		}

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

		bool onMouseMoved(MouseMovedEvent& e)
		{
			return m_layerStack->OnMouseMoved(e);
		}

		bool onMousePressed(MouseButtonPressedEvent& e)
		{
			return m_layerStack->OnMousePressed(e);
		}

		bool onMouseReleased(MouseButtonReleasedEvent& e)
		{
			e.handle(true);
			auto button = e.getButton();
			Log::info("Mouse Button Released event: ({0})", button);
			m_poller->processMouseClick(button, 0, 0);
			return e.handled();
		}

		bool onMouseWheel(MouseScrolledEvent& e)
		{
			e.handle(true);
			auto xOffset = e.getXOffset();
			auto yOffset = e.getYOffset();
			Log::info("Mouse Moved event: ({0}, {1})", xOffset, yOffset);
			m_poller->processMouseScroll(xOffset, yOffset);
			return e.handled();
		}

	private:
		static Application* s_instance; //!< Singleton instance of the application
		bool m_running = true; //!< Is the application running?
		bool m_focus = true;

	public:
		virtual ~Application(); //!< Deconstructor
		inline static Application& getInstance() { return *s_instance; } //!< Instance getter from singleton pattern
		inline static std::shared_ptr<Window> getWindow() { return s_instance->m_window; }
		void run(); //!< Main loop
	};

	// To be defined in users code
	Application* startApplication(); //!< Function definition which provides an entry hook
}
