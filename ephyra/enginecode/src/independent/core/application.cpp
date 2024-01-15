/** \file application.cpp */

#include "engine_pch.h"
#include "core/application.h"
#include "core/Scenes/SceneManager.h"

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

		// reset timer
		m_timer.reset(new Engine::ChronoTimer);
		m_timer->start();
		deltaTime = m_timer->getElapsedTime();
		m_timer->reset();

		m_running = true;
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		std::shared_ptr<SceneManager> sceneManager;
		sceneManager = std::make_shared<Engine::SceneManager>();
		m_window = sceneManager->getWindow();

		sceneManager->LoadScene(std::string("Intro Scene"), std::make_shared<Engine::IntroScene>(sceneManager->getWindow(), sceneManager->getPoller()));
		sceneManager->SetActiveScene(std::string("Intro Scene"));

		while (m_running)
		{
			m_running = sceneManager->UpdateActiveScene(deltaTime);
			deltaTime = m_timer->getElapsedTime();
			m_timer->reset();
		}
	}

}
