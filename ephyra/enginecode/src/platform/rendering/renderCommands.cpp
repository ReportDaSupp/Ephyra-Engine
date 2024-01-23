/**  \file renderCommands.cpp */

#include "ephyra_pch.h"

#include "rendering/renderCommands.h"
#include "rendering/renderAPI.h"
#include <glad/glad.h>







namespace Ephyra
{
	std::function<void(void)> RenderCommandFactory::getClearColourAndDepthBufferCommand()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::Direct3D:
			return std::function<void(void)>();
		
		case RenderAPI::API::None:
			return std::function<void(void)>();
		
		case RenderAPI::API::OpenGL:
			return []() {glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); };
	
		case RenderAPI::API::Vulkan:
			return std::function<void(void)>();

		default:
			return std::function<void(void)>();

		}
	}

	std::function<void(void)> RenderCommandFactory::setClearColourCommand(float r, float g, float b, float a)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::Direct3D:
			return std::function<void(void)>();

		case RenderAPI::API::None:
			return std::function<void(void)>();

		case RenderAPI::API::OpenGL:
			return [r, g, b, a]() { glClearColor(r, g, b, a); };

		case RenderAPI::API::Vulkan:
			return std::function<void(void)>();

		default:
			return std::function<void(void)>();

		}
	}

	std::function<void(void)> RenderCommandFactory::glEnableCommand(int&& command)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::Direct3D:
			return std::function<void(void)>();

		case RenderAPI::API::None:
			return std::function<void(void)>();

		case RenderAPI::API::OpenGL:
			return [command]() { glEnable(command); };

		case RenderAPI::API::Vulkan:
			return std::function<void(void)>();

		default:
			return std::function<void(void)>();

		}
	}

	std::function<void(void)> RenderCommandFactory::glDisableCommand(int&& command)
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::Direct3D:
			return std::function<void(void)>();

		case RenderAPI::API::None:
			return std::function<void(void)>();

		case RenderAPI::API::OpenGL:
			return [command]() { glDisable(command); };

		case RenderAPI::API::Vulkan:
			return std::function<void(void)>();

		default:
			return std::function<void(void)>();

		}
	}

	std::function<void(void)> RenderCommandFactory::setBlendFuncCommand()
	{
		switch (RenderAPI::getAPI())
		{
		case RenderAPI::API::Direct3D:
			return std::function<void(void)>();

		case RenderAPI::API::None:
			return std::function<void(void)>();

		case RenderAPI::API::OpenGL:
			return []() { glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); };

		case RenderAPI::API::Vulkan:
			return std::function<void(void)>();

		default:
			return std::function<void(void)>();

		}
	}
}