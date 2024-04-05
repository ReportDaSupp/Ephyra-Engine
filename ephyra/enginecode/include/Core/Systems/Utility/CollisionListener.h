#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include <Ephyra.h>

class CollisionListener : public reactphysics3d::EventListener
{
public:
	void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override;

	void onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData) override;
};

void CollisionListener::onContact(const rp3d::CollisionCallback::CallbackData& callbackData)
{
	std::cout << "Collision Detected!" << std::endl;
};

void CollisionListener::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData)
{
	std::cout << "Trigger Detected!" << std::endl;
};