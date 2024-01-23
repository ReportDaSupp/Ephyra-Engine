#pragma once

// entry point
#include "include/independent/entrypoint.h"
#include "ephyra.h"

class animationApp : public Ephyra::Application
{
public:
	animationApp();
	~animationApp() override;
};
