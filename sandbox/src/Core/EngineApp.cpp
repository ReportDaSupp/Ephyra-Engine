/** \file engineApp.cpp
*/
#include "Core/engineApp.h"
#include "Layers/GameLayer.h"
#include "Layers/ImGuiLayer.h"


engineApp::engineApp()
{
	m_layerStack->Push(std::make_shared<GameLayer>(GameLayer("GameLayer")));
	m_layerStack->Push(std::make_shared<ImGuiLayer>(ImGuiLayer("ImGuiLayer")));
}

engineApp::~engineApp()
{


}

Engine::Application* Engine::startApplication()
{
	return new engineApp();
}