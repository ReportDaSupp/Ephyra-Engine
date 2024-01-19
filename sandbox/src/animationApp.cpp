/** \file animationApp.cpp
*/
#include "animationApp.h"

animationApp::animationApp()
{
}

animationApp::~animationApp()
{

}

Engine::Application* Engine::startApplication()
{
	return new animationApp();
}