#pragma once

#include "Core/Initialization/EntryPoint.h"
#include "Ephyra.h"

class engineApp : public Engine::Application
{
public:
	engineApp();
	~engineApp() override;
};
