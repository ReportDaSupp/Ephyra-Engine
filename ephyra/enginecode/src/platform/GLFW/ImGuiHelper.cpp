#include "engine_pch.h"


#include <examples/imgui_impl_opengl3.cpp>
#include <examples/imgui_impl_glfw.cpp>

#include "platform/GLFW/ImGuiHelper.h"
#include "core/application.h"

namespace Engine
{
	std::string s_iniFileLocation = std::string("../../tool_imgui.ini");

	void ImGuiHelper::init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		auto& window = Application::getInstance().getWindow();

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window->getNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 440");
	}

	void ImGuiHelper::begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiHelper::end()
	{
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiHelper::shutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}



