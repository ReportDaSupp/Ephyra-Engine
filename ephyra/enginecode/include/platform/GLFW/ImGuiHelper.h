#pragma once
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

namespace Engine
{
	class ImGuiHelper
	{
	public:
		static void init();
		static void begin();
		static void end();
		static void shutdown();
	private:
	static std::string s_iniFileLocation;
	};
}
