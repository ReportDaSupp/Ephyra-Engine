#pragma once
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <misc/cpp/imgui_stdlib.h>
#include "rendering/rendererCommon.h"

namespace Ephyra
{
	struct ImGuiData
	{
		bool editable = false;
		bool eMenu = true;
		int numModels = 1;
		int selectedLight = 1;
		std::string assetPath = "./assets/models/Suzanne.obj";
		std::string texturePath = "assets/textures/Grime.png";
		std::vector<PointLight> Lights;
	};

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
