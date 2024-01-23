#pragma once

#include "ephyra.h"

extern Ephyra::Application* Ephyra::startApplication();

int main(int argc, char** argv)
{
	auto application = Ephyra::startApplication();
	application->run();
	
	delete application;

	return 0;
}
