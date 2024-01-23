/**
 * \file application.h
 * \brief Fundemental class of the engine. A singleton which runs the game loop infinitely.
 * 
 * This file contains the declaration of the Application class, which is the fundamental class of the engine. It is a singleton class that runs the game loop infinitely. It provides various functionalities for the engine.
 */

#pragma once

#include <functional>

#include "systems/log.h"
#include "core/timer.h"
#include "systems/randomNumGen.h"
#include "events/events.h"
#include "core/window.h"
#include "core/inputPoller.h"
#include "systems/cameraFPS.h"

namespace Ephyra {

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

		std::shared_ptr<Window> m_window; //!< Window
		std::shared_ptr<Timer> m_timer; //! Timer
		std::shared_ptr<InputPoller> m_poller;

		float deltaTime;

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
