#pragma once

#include "Core/Systems/Utility/System.h"
#include "Core/Systems/Utility/CollisionListener.h"

#include <reactphysics3d/reactphysics3d.h>

namespace Engine
{
	class PhysicsSystem : public System
	{
	public:
		void start(SystemSignal init = SystemSignal::None, ...) override
		{
			m_world = m_rp3dCommon.createPhysicsWorld();
		}

		void stop(SystemSignal close = SystemSignal::None, ...) override
		{
			m_rp3dCommon.destroyPhysicsWorld(m_world);
		}

		void update(float timestep) {
			m_world->update(timestep);
		}

		rp3d::PhysicsWorld* getPhysicsWorld()
		{
			return m_world;
		}

		rp3d::PhysicsCommon& getPhysicsCommon()
		{
			return m_rp3dCommon;
		}

	private:
		rp3d::PhysicsWorld* m_world;
		rp3d::PhysicsCommon m_rp3dCommon;
	};
}