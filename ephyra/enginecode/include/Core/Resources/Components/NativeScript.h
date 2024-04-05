#pragma once



#include "Core/Systems/Events/Events.h"
#include "Core/Resources/Management/ResourceManager.h"

#include <memory>
#include <src/entt/entt.hpp>

namespace Engine {
	class NativeScript {
	public:
		NativeScript() : m_entity(entt::null), m_registry(ResourceManager::getInstance()->m_registry) {};
		NativeScript(entt::entity entity) : m_entity(entity), m_registry(ResourceManager::getInstance()->m_registry) {};
		virtual void onUpdate(float timestep) = 0;
		virtual void onKeyPress(KeyPressedEvent& e)= 0;
		

	protected:
		entt::entity m_entity;
		entt::registry& m_registry;
	};

	class NativeScriptComponent {
	public:
		NativeScriptComponent() = default;
		~NativeScriptComponent() = default;
		void onUpdate(float timestep) { m_instance->onUpdate(timestep); }
		void onKeyPress(KeyPressedEvent& e) { m_instance->onKeyPress(e); }
		template <typename G, typename ...Args> void create(Args&& ... args) {
			G* ptr = new (std::forward<Args>(args)...);
			m_instance.reset(static_cast<NativeScript*>(ptr));
		}
	protected:
		std::shared_ptr<NativeScript> m_instance;
	};
}