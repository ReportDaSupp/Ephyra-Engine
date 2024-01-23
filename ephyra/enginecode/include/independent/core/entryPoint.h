/** \file entryPoint.h
 * \brief This file contains the entry point of the application.
 */

#pragma once

#include "core/application.h"

/**
 * \brief Function to start the application.
 * \return A pointer to the application object.
 */
extern Ephyra::Application* Ephyra::startApplication();

/**
 * \brief The main function of the application.
 * \param argc The number of command-line arguments.
 * \param argv An array of command-line arguments.
 * \return The exit code of the application.
 */
int main(int argc, char** argv)
{
	auto application = Ephyra::startApplication();
	application->run();
	delete application;

	return 0;
}
