/**
 * \file application.h
 * \brief Fundemental class of the engine. A singleton which runs the game loop infinitely.
 * 
 * This file contains the declaration of the Application class, which is the fundamental class of the engine. It is a singleton class that runs the game loop infinitely. It provides various functionalities for the engine.
 */

#pragma once

#include <functional>

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

	private:
		static Application* s_instance; //!< Singleton instance of the application

	public:
		virtual ~Application(); //!< Deconstructor
		inline static Application& getInstance() { return *s_instance; } //!< Instance getter from singleton pattern
		void run(); //!< Main loop
	};

	// To be defined in users code
	Application* startApplication(); //!< Function definition which provides an entry hook
}
