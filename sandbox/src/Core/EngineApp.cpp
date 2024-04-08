/** \file engineApp.cpp
*/
#include "Core/engineApp.h"
#include "Layers/EngineLayer.h"
#include "Layers/ImGuiLayer.h"


engineApp::engineApp()
{
	m_layerStack->Push(std::make_shared<EngineLayer>(EngineLayer("EngineLayer")));
	m_layerStack->Push(std::make_shared<ImGuiLayer>(ImGuiLayer("ImGuiLayer")));
}

engineApp::~engineApp()
{


}

Engine::Application* Engine::startApplication()
{
	return new engineApp();
}