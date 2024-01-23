/** \file animationApp.cpp
*/
#include "animationApp.h"

animationApp::animationApp()
{
}

animationApp::~animationApp()
{

}

Ephyra::Application* Ephyra::startApplication()
{
	return new animationApp();
}