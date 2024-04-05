#pragma once
#include <vector>
#include <memory>
#include "Core/Resources/Utility/Layer.h"

namespace Engine
{

    class LayerStack {
    public:
        LayerStack(std::shared_ptr<Window> window, std::shared_ptr<InputPoller> poller) { m_window = window; m_poller = poller; };
        ~LayerStack();

        void Push(const std::shared_ptr<Layer>& layer);
        void Pop();

        std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
        std::vector<std::shared_ptr<Layer>>::iterator end() { return m_layers.end(); }

        bool OnKeyPress(Engine::KeyPressedEvent& e);
        bool OnMousePressed(Engine::MouseButtonPressedEvent& e);
        bool OnMouseMoved(Engine::MouseMovedEvent& e);

    private:
        std::vector<std::shared_ptr<Layer>> m_layers;
        std::shared_ptr<Window> m_window;
        std::shared_ptr<InputPoller> m_poller;
    };

    inline LayerStack::~LayerStack() {
        for (auto& layer : m_layers) {
            layer->OnDetach();
        }
    }

    inline void LayerStack::Push(const std::shared_ptr<Layer>& layer) {
        m_layers.emplace_back(layer);
        layer->bindHandlers(m_window, m_poller);
        layer->OnAttach();
    }

    inline void LayerStack::Pop() {
        if (!m_layers.empty()) {
            m_layers.back()->OnDetach();
            m_layers.pop_back();
        }
    }
    inline bool LayerStack::OnKeyPress(Engine::KeyPressedEvent& e) {
        for (auto layer : m_layers)
        { 
            if (layer->isFocus() && !e.handled()) layer->OnKeyPress(e);
        }
        return e.handled();
    }
    inline bool LayerStack::OnMousePressed(Engine::MouseButtonPressedEvent& e) {
        for (auto layer : m_layers)
        {
            if (layer->isFocus() && !e.handled()) layer->OnMousePressed(e);
        }
        return e.handled();
    }
    inline bool LayerStack::OnMouseMoved(Engine::MouseMovedEvent& e) {
        for (auto layer : m_layers)
        {
            if (layer->isFocus() && !e.handled()) layer->OnMouseMovedEvent(e);
        }
        return e.handled();
    }
}