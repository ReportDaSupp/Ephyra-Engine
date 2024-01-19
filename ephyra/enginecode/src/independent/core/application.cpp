/** \file application.cpp */

#include "ephyra_pch.h"
#include "core/application.h"


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
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		
	}

}
