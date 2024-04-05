#pragma once
#include <memory>
#include <string>
#include "Core/Systems/Events/Events.h"

namespace Engine
{

    // Not yet implemented classes (To be created with ECS)
    class ResourceManager;
    class Renderer;
    class CameraController;

    class Layer {
    public:
        Layer(const std::string& name) { m_name = name; };
        virtual ~Layer() { this->OnDetach(); };

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        
        virtual void OnUpdate(float timestep) {}
        virtual void OnRender() {}
        virtual bool OnKeyPress(Engine::KeyPressedEvent& e) { return false; }
        virtual bool OnMousePressed(Engine::MouseButtonPressedEvent& e) { return false; }
        virtual bool OnMouseMovedEvent(Engine::MouseMovedEvent& e) { return false; }

        virtual void bindHandlers(std::shared_ptr<Engine::Window> window, std::shared_ptr<Engine::InputPoller> poller) {};
        virtual bool isFocus() { return true; };

        const std::string& GetName() const { return m_name; }

    protected:
        std::string m_name;
        std::shared_ptr<ResourceManager> m_resources;
        std::shared_ptr<Renderer> m_renderer;
        std::shared_ptr<CameraController> m_camera;
    };
}