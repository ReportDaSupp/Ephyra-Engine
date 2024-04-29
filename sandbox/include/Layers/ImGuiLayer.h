#pragma once

#include "Ephyra.h"

#include "Core/Resources/Management/ResourceManager.h"
#include "Core/Rendering/API/Global/RendererCommon.h"
#include "Core/Resources/Management/SceneManager.h"
#include "Core/Systems/Events/InputPoller.h"

#include <External/IMGui/imgui.h>
#include <External/IMGui/imgui_impl_glfw.h>
#include <External/IMGui/imgui_impl_opengl3.h>
#include <External/IMGui/imgui_stdlib.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <Platform/OpenGl/OpenGLTexture.h>


void SetCustomImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.129f, 0.133f, 0.149f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.165f, 0.231f, 0.259f, 1.0f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.722f, 0.322f, 0.039f, 1.0f);
    //style.Colors[ImGuiCol_] = ImVec4(, 1.0f);
    //style.Colors[ImGuiCol_] = ImVec4(, 1.0f);
    //style.Colors[ImGuiCol_] = ImVec4(, 1.0f);
    //style.Colors[ImGuiCol_] = ImVec4(, 1.0f);
    //style.Colors[ImGuiCol_] = ImVec4(, 1.0f);
}


class ImGuiLayer : public Engine::Layer {
private:
    // Add all game data here prior to implementation of resource manager
    std::shared_ptr<Engine::ResourceManager> gResources;
    std::shared_ptr<Engine::SceneManager> gManager;

    ImVec4 TitleColor = { 1.f, 0.5f, 0.f, 1.f };
    ImVec4 SubTitleColor = { 0.3f, 0.3f, 1.f, 1.f };

    bool isDragging = false;
    ImVec2 dragStartPos;
    int winPosX;
    int winPosY;

    float timelineStart = 0.0f;
    float timelineEnd = 250.0f;
    bool  isPlaying = false;
    float maxTime = 250.0f;

    ImVec2 ViewportSize;

public:
    ImGuiLayer(const std::string& name = "ImGuiLayer")
        : Engine::Layer(name) { gResources = Engine::ResourceManager::getInstance(); gManager = Engine::SceneManager::getInstance();}

    void OnAttach() override;
    void OnDetach() override;
    void bindHandlers(std::shared_ptr<Engine::Window> window, std::shared_ptr<Engine::InputPoller> poller) override {};
    void OnUpdate(float timestep) override;
    void OnRender() override;
    bool OnKeyPress(Engine::KeyPressedEvent& e) override;
    bool OnMousePressed(Engine::MouseButtonPressedEvent& e) override;
    bool OnMouseMovedEvent(Engine::MouseMovedEvent& e) override;
};

void ImGuiLayer::OnAttach() {
    // Initialization code here
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGuiIO& io = ImGui::GetIO();
    
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)gResources->m_window->getNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 440");
    ImGui::StyleColorsDark();
    SetCustomImGuiStyle();

    gResources->imGuiWelcomeImage.reset(Engine::Texture::create("./assets/sprites/WelcomeImage.png"));
    gResources->texViewerImage = Engine::RendererCommon::defaultTexture;
}

void ImGuiLayer::OnDetach() {
    // Cleanup code here
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnUpdate(float timestep) {

    SCR_WIDTH = ViewportSize.x;
    SCR_HEIGHT = ViewportSize.y;

    if (gResources->m_poller->keyHasBeenPressed())
    {
        if (gResources->m_poller->isKeyPressed(GLFW_KEY_TAB)) {
            gResources->isGuiActive = false;
        }
        if (gResources->m_poller->isKeyPressed(GLFW_KEY_LEFT_ALT)) {
            gResources->isGuiActive = true;
        }
    }

    // Update logic for each frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    if (gResources->isGuiActive) glfwSetInputMode((GLFWwindow*)gResources->m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else glfwSetInputMode((GLFWwindow*)gResources->m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (isPlaying)
    {
        gResources->currentTimeKey += timestep;
    }
    if (gResources->currentTimeKey > maxTime)
    {
        gResources->currentTimeKey = 0;
        isPlaying = false;
    }

    this->OnRender();
}

void ImGuiLayer::OnRender() {

    Engine::RendererCommon::colorFBO->unbind();

    float ms = 1000.0f / ImGui::GetIO().Framerate;

    if (gResources->isGuiActive)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.129f, 0.133f, 0.149f, 1.0f);
        
    }
    else
    {
        ImGuiStyle& style = ImGui::GetStyle();

        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.133f, 0.149f, 1.0f);
    }

    if (gResources->eIntroMessage)
    {
        if (ImGui::Begin("Welcome Screen", &gResources->eIntroMessage, ImGuiWindowFlags_NoDecoration))
        {   
            ImGui::Text("Welcome to the Ephyra Animation Engine!");

            gResources->imGuiWelcomeImage->load(0);
            ImGui::Image((void*)(intptr_t)gResources->imGuiWelcomeImage->getID(), ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x), ImVec2(0, 0), ImVec2(1, 1));

            static char loadName[128] = "";
            ImGui::InputTextWithHint("Load Name: ", "Enter Scene Name", loadName, IM_ARRAYSIZE(loadName));
            if (ImGui::Button("Load Scene From File"))
            {
                gManager->loadScene(gResources->m_registry, loadName);
                std::string name = "Ephyra Animation Engine - ";
                for (int i = 0; i < IM_ARRAYSIZE(loadName); i++)
                {
                    name += loadName[i];
                }
                glfwSetWindowTitle((GLFWwindow*)gResources->m_window->getNativeWindow(), name.c_str());
                gResources->eIntroMessage = false;
                /*for (int i = 0; i < 25; i++)
                {
                    std::string assetName = "Grass" + std::to_string(i);
                    auto asset = gResources->m_registry.create();
                    gResources->m_registry.emplace<Engine::TagComponent>(asset, assetName, Engine::TagType::Render3D);
                    gResources->m_registry.emplace<Engine::StateComponent>(asset, true);
                    gResources->m_registry.emplace<Engine::TransformComponent>(asset, glm::vec3{ (i % 5), 0.6, (float)std::floor(i/5) }, glm::vec3{ 0,0,0 }, glm::vec3{ 0.05, 0.3, 0.05 });
                    gResources->m_registry.emplace<Engine::MeshRendererComponent>(asset, "./assets/models/Cube/Cube.obj", assetName);
                }*/
            }
            
        }
        ImGui::End();
    }

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGuiWindowFlags windowFlags = 0;
    static bool showDecoration = false;
    if (!showDecoration)
        windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

    if (gResources->eHierarchy)
    {
        if (ImGui::Begin("Scene Hierarchy", &gResources->eHierarchy, windowFlags))
        {
            ImGui::TextColored(SubTitleColor, "3D Objects: ");
            auto view = gResources->m_registry.view<Engine::TagComponent>();
            for (auto tagEntity : view)
            {
                auto& tag = view.get<Engine::TagComponent>(tagEntity);
                if (tag.Type == Engine::TagType::Render3D)
                {
                    if (ImGui::Button(tag.Tag.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
                    {
                        gResources->selectedObject = tag.Tag;
                    }
                }
            }
            ImGui::Separator();
            ImGui::TextColored(SubTitleColor, "2D Objects: ");
            for (auto tagEntity : view)
            {
                auto& tag = view.get<Engine::TagComponent>(tagEntity);
                if (tag.Type == Engine::TagType::Render2D)
                {
                    if (ImGui::Button(tag.Tag.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
                    {
                        gResources->selectedObject = tag.Tag;
                    }
                }
            }
            ImGui::Separator();
            ImGui::TextColored(SubTitleColor, "Lights: ");
            for (auto tagEntity : view)
            {
                auto& tag = view.get<Engine::TagComponent>(tagEntity);
                if (tag.Type == Engine::TagType::Light)
                {
                    if (ImGui::Button(tag.Tag.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
                    {
                        gResources->selectedObject = tag.Tag;
                    }
                }
            }
            ImGui::Separator();
            auto tagView = gResources->m_registry.view<Engine::TagComponent>();
            for (auto asset : tagView)
            {
                auto& tag = tagView.get<Engine::TagComponent>(asset);
                if (tag.Tag == gResources->selectedObject)
                { 
                    ImGui::Text("");
                    ImGui::TextColored(TitleColor, tag.Tag.c_str());

                    if (gResources->m_registry.all_of<Engine::TransformComponent>(asset))
                    {
                        ImGui::TextColored(SubTitleColor, "Transformation");
                        auto localView = gResources->m_registry.view<Engine::TransformComponent>();
                        auto& transformation = localView.get<Engine::TransformComponent>(asset);
                        ImGui::DragFloat3("Translation: ", &transformation.Translation.x, 0.05f);
                        ImGui::DragFloat3("Rotation: ", &transformation.Euler.x, 0.05f);
                        ImGui::DragFloat3("Scale: ", &transformation.Scale.x, 0.05f);

                        glm::mat4 T = glm::translate(glm::mat4(1.0), transformation.Translation);
                        glm::mat4 R = glm::mat4_cast(glm::quat(transformation.Euler));
                        transformation.Rotation = R;
                        glm::mat4 S = glm::scale(glm::mat4(1.0), transformation.Scale);

                        transformation.Transform = T * R * S;
                        if (gResources->m_registry.all_of<Engine::RigidBodyComponent>(asset))
                        {
                            auto& rbc = gResources->m_registry.get<Engine::RigidBodyComponent>(asset);
                            reactphysics3d::Transform transform;
                            transform.setPosition({ transformation.Translation.x,  transformation.Translation.y,  transformation.Translation.z });
                            transform.setOrientation({ transformation.Rotation.x, transformation.Rotation.y, transformation.Rotation.z, transformation.Rotation.w });
                            rbc.m_body->setTransform(transform);
                        }
                    }
                    if (gResources->m_registry.all_of<Engine::EmmissiveComponent>(asset))
                    {
                        ImGui::TextColored(SubTitleColor, "Light Transform");
                        auto localView = gResources->m_registry.view<Engine::EmmissiveComponent>();
                        auto& transformation = localView.get<Engine::EmmissiveComponent>(asset);
                        ImGui::DragFloat3("Relative Position: ", &transformation.Position.x, 0.05f);
                        ImGui::DragFloat3("Color: ", &transformation.Color.x, 0.05f);
                    }
                    if (gResources->m_registry.all_of<Engine::StateComponent>(asset))
                        ImGui::Checkbox("Visible: ", &gResources->m_registry.get<Engine::StateComponent>(asset).State);
                    ImGui::Separator();
                    if (ImGui::Button("Delete Asset", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
                    {
                        gResources->m_registry.destroy(asset);
                    }
                }
            }
        }
        ImGui::End();
    }

    if (gResources->eViewport)
    {
        if (ImGui::Begin("Viewport", &gResources->eViewport, windowFlags)) {
            uint32_t temp;

            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            ImVec2 currentViewportSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

            float originalImageWidth = 1920;
            float originalImageHeight = 1080;
            float aspectRatio = originalImageWidth / originalImageHeight;

            float scaledWidth, scaledHeight;
            if (currentViewportSize.x / currentViewportSize.y > aspectRatio) {
                scaledHeight = currentViewportSize.y;
                scaledWidth = scaledHeight * aspectRatio;
            }
            else {
                scaledWidth = currentViewportSize.x;
                scaledHeight = scaledWidth / aspectRatio;
            }

            ImVec2 imageRenderSize = ImVec2(scaledWidth, scaledHeight);

            ImGui::Image((void*)(intptr_t)Engine::RendererCommon::colorFBOTexture->getID(), imageRenderSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
    }
    if (gResources->eTextureViewer)
    {
        if (ImGui::Begin("Texture Viewer", &gResources->eTextureViewer, windowFlags)) {
            uint32_t temp;

            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;
            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            ImVec2 currentViewportSize = ImVec2(vMax.x - vMin.x, vMax.y - vMin.y);

            float originalImageWidth = gResources->texViewerImage->getWidthf();
            float originalImageHeight = gResources->texViewerImage->getHeightf();
            float aspectRatio = originalImageWidth / originalImageHeight;

            float scaledWidth, scaledHeight;
            if (currentViewportSize.x / currentViewportSize.y > aspectRatio) {
                scaledHeight = currentViewportSize.y;
                scaledWidth = scaledHeight * aspectRatio;
            }
            else {
                scaledWidth = currentViewportSize.x;
                scaledHeight = scaledWidth / aspectRatio;
            }

            ImVec2 imageRenderSize = ImVec2(scaledWidth, scaledHeight);

            ImVec2 viewerStartPos = ImGui::GetCursorScreenPos();
            ImVec2 viewerSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

            ImGui::Image((void*)(intptr_t)gResources->texViewerImage->getID(), imageRenderSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
    }
    if (gResources->eKeyframe)
    {
        if (ImGui::Begin("Keyframe Timeline"))
        {

            // Button Size
            ImVec2 buttonSize(50, 20);

            // Play Button
            if (ImGui::Button(isPlaying ? "Pause" : "Play", buttonSize)) {
                isPlaying = !isPlaying; // Toggle play/pause state
            }
            ImGui::SameLine();

            if (ImGui::Button("Reset", buttonSize)) {
                gResources->currentTimeKey = 0.0f;
            }
            ImGui::SameLine();

            ImGui::PushItemWidth(100);
            ImGui::InputFloat("Duration", &maxTime, 1.0f, 10.0f, "%.1f");
            ImGui::PopItemWidth();

            float windowVisible = ImGui::GetContentRegionAvail().x;
            float timelineWidth = windowVisible - buttonSize.x * 3 - ImGui::GetStyle().ItemSpacing.x * 4;

            ImGui::Text("Current Time: %.2f", gResources->currentTimeKey);
            ImGui::SameLine();
            ImGui::Text("Maximum Time: %.2f", maxTime);
            

            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            ImVec2 timelineStartPos = ImGui::GetCursorScreenPos();
            ImVec2 timelineSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

            draw_list->AddRectFilled(timelineStartPos, ImVec2(timelineStartPos.x + timelineSize.x, timelineStartPos.y + timelineSize.y), IM_COL32(80, 80, 80, 255));

            int numTicks = maxTime;
            for (int i = 0; i <= numTicks; ++i) {
                float t = (float)i / (float)numTicks;
                float x = timelineStartPos.x + t * timelineSize.x;
                if (i % 5 == 0)
                    draw_list->AddLine(ImVec2(x, timelineStartPos.y), ImVec2(x, timelineStartPos.y + timelineSize.y), IM_COL32(160, 160, 160, 255));
                else 
                    draw_list->AddLine(ImVec2(x, timelineStartPos.y), ImVec2(x, timelineStartPos.y + timelineSize.y), IM_COL32(60, 60, 60, 255));
            }

            float t = (gResources->currentTimeKey - timelineStart) / (timelineEnd - timelineStart);
            float x = timelineStartPos.x + t * timelineSize.x;
            draw_list->AddLine(ImVec2(x, timelineStartPos.y), ImVec2(x, timelineStartPos.y + timelineSize.y), IM_COL32(255, 255, 0, 255), 2.0f);

            ImGui::InvisibleButton("##timeline", timelineSize);
            if (ImGui::IsItemHovered()) {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    float newT = (mousePos.x - timelineStartPos.x) / timelineSize.x;
                    gResources->currentTimeKey = timelineStart + newT * (timelineEnd - timelineStart);
                }
            }

            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 mousePos = ImGui::GetMousePos();
                float newT = (mousePos.x - timelineStartPos.x) / timelineSize.x;
                gResources->currentTimeKey = timelineStart + newT * (timelineEnd - timelineStart);
            }

            ImGui::End();
        }
    }
    if (gResources->eAssets)
    {
        if (ImGui::Begin("Textures", &gResources->eAssets, windowFlags))
        {
            for (auto temp : gResources->getAll())
            {
                switch (temp.type)
                {
                    case Engine::SceneAsset::Type::Undefined:
                    {
                        //ImGui::Text("Undefined: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Transform:
                    {
                        //ImGui::Text("Transform: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Quad:
                    {
                        //ImGui::Text("Quad: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Shader:
                    {
                        //ImGui::Text("Shader: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Texture:
                    {   

                        const ImVec2 buttonSize(96, 96);
                        const float padding = 10.0f;

                        // Calculate total item width and height
                        ImVec2 textSize = ImGui::CalcTextSize(temp.id.c_str());
                        float itemWidth = buttonSize.x + padding * 2 + 6;
                        float itemHeight = buttonSize.y + padding + 6 + textSize.y;

                        ImVec2 cursorPos = ImGui::GetCursorScreenPos(); // Store cursor position

                        // Container rectangle for the current item
                        ImDrawList* drawList = ImGui::GetWindowDrawList();
                        ImVec2 rectMin = cursorPos;
                        ImVec2 rectMax = ImVec2(cursorPos.x + itemWidth, cursorPos.y + itemHeight);
                        drawList->AddRectFilled(rectMin, rectMax, IM_COL32(50, 50, 50, 255), 1.0f);

                        // Offset cursor for image button
                        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + padding, cursorPos.y + padding));
                        if (ImGui::ImageButton((ImTextureID)gResources->getAsset<Engine::Texture>(temp.id)->getID(), buttonSize, ImVec2(0,0), ImVec2(1, 1), 3)) {
                            gResources->eTextureViewer = true;
                            gResources->texViewerImage = gResources->getAsset<Engine::Texture>(temp.id);
                        }

                        // Drag and drop source for the image button
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                            ImGui::SetDragDropPayload("TEXTURE_PAYLOAD", &temp.id, sizeof(temp.id)); // Set payload to carry the texture identifier
                            ImGui::Text("Texture: %s", temp.id); // Display texture name as the drag preview
                            ImGui::EndDragDropSource();
                        }

                        // Position the cursor for the text label below the image button
                        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + padding, cursorPos.y + buttonSize.y + padding + 6));
                        ImGui::Text("%s", temp.id);

                        if (ImGui::GetContentRegionAvail().x < cursorPos.x - ImGui::GetWindowPos().x + itemWidth * 2 + ImGui::GetStyle().ItemSpacing.x)
                        {
                            ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, cursorPos.y + itemHeight));
                            ImGui::NewLine();
                        }
                        else
                            ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + itemWidth + ImGui::GetStyle().ItemSpacing.x, cursorPos.y));

                        break;
                    }
                    case Engine::SceneAsset::Type::SubTexture:
                    {
                        //ImGui::Text("SubTexture: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Material:
                    {
                        /*ImGui::Text("Material: %s", temp.id);
                        auto& textures = gResources->getAsset<Engine::Material>(temp.id)->getTextures();
                        for (int i = 0; i < textures.size(); i++)
                            ImGui::Text("Textures: %i", textures[i]->getID());*/

                        break;
                    }
                    case Engine::SceneAsset::Type::Geometry:
                    {
                        //ImGui::Text("Geometry: %s", temp.id);
                        break;
                    }
                }


                
            }
        }
        ImGui::End();
    }

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::BeginMenu("Open: "))
            {
                static char loadName[128] = "";
                ImGui::InputTextWithHint("Load Name: ", "Enter Scene Name", loadName, IM_ARRAYSIZE(loadName));
                if (ImGui::Button("Load Scene"))
                {
                    gManager->loadScene(gResources->m_registry, loadName);
                    std::string name = "Ephyra Animation Engine - ";
                    for (int i = 0; i < IM_ARRAYSIZE(loadName); i++)
                    {
                        name += loadName[i];
                    }
                    glfwSetWindowTitle((GLFWwindow*)gResources->m_window->getNativeWindow(), name.c_str());
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Save: "))
            {
                static char saveName[128] = "";
                ImGui::InputTextWithHint("Save Name: ", "Enter Scene Name", saveName, IM_ARRAYSIZE(saveName));
                if (ImGui::Button("Save Scene"))
                {
                    gManager->saveScene(gResources->m_registry, saveName);
                    std::string name = "Ephyra Animation Engine - ";
                    for (int i = 0; i < IM_ARRAYSIZE(saveName); i++)
                    {
                        name += saveName[i];
                    }
                    glfwSetWindowTitle((GLFWwindow*)gResources->m_window->getNativeWindow(), name.c_str());
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Load Asset: "))
            {
                static char assetPath[128] = "./assets/models/";
                static char assetName[128] = "";
                static glm::vec3 Translation = { 0, 0, 0 };
                static glm::vec3 Rotation = { 0, 0, 0 };
                static glm::vec3 Scale = { 1, 1, 1 };
                static bool safeName = true;
                ImGui::InputTextWithHint("Asset Path: ", "Enter Asset Filepath", assetPath, IM_ARRAYSIZE(assetPath));
                ImGui::InputTextWithHint("Asset Name: ", "Enter Asset Name", assetName, IM_ARRAYSIZE(assetName));
                ImGui::InputFloat3("Translation: ", &Translation.x);
                ImGui::InputFloat3("Rotation: ", &Rotation.x);
                ImGui::InputFloat3("Scale: ", &Scale.x);
                if (safeName)
                {
                    if (ImGui::Button("Load Asset"))
                    {
                        auto view = gResources->m_registry.view<Engine::TagComponent>();
                        for (auto entity : view)
                            if (assetName == gResources->m_registry.get<Engine::TagComponent>(entity).Tag)
                            {
                                safeName = false;
                            }
                        if (safeName)
                        {
                            auto asset = gResources->m_registry.create();
                            gResources->m_registry.emplace<Engine::TagComponent>(asset, assetName, Engine::TagType::Render3D);
                            gResources->m_registry.emplace<Engine::StateComponent>(asset, true);
                            gResources->m_registry.emplace<Engine::TransformComponent>(asset, Translation, Rotation, Scale);
                            gResources->m_registry.emplace<Engine::MeshRendererComponent>(asset, assetPath, assetName);
                            Translation = { 0, 0, 0 };
                            Rotation = { 0, 0, 0 };
                            Scale = { 1, 1, 1 };
                            //gResources->m_registry.emplace<Engine::RigidBodyComponent>(asset, asset, rp3d::BodyType::STATIC);
                            //gResources->m_registry.emplace<Engine::BoxColliderComponent>(asset, asset, glm::vec3(88.f, 1.0f, 68.f));
                        }
                    }
                }
                else if (ImGui::Button("Invalid Name"))
                {
                    safeName = true;
                    auto view = gResources->m_registry.view<Engine::TagComponent>();
                    for (auto entity : view)
                        if (assetName == gResources->m_registry.get<Engine::TagComponent>(entity).Tag)
                        {
                            safeName = false;
                        }
                    if (safeName)
                    {
                        auto asset = gResources->m_registry.create();
                        gResources->m_registry.emplace<Engine::TagComponent>(asset, assetName, Engine::TagType::Render3D);
                        gResources->m_registry.emplace<Engine::StateComponent>(asset, true);
                        gResources->m_registry.emplace<Engine::TransformComponent>(asset, Translation, Rotation, Scale);
                        gResources->m_registry.emplace<Engine::MeshRendererComponent>(asset, assetPath, assetName);
                        Translation = { 0, 0, 0 };
                        Rotation = { 0, 0, 0 };
                        Scale = { 1, 1, 1 };
                        //gResources->m_registry.emplace<Engine::RigidBodyComponent>(asset, asset, rp3d::BodyType::STATIC);
                        //gResources->m_registry.emplace<Engine::BoxColliderComponent>(asset, asset, glm::vec3(88.f, 1.0f, 68.f));
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Render"))
        {        
            ImGui::Checkbox("Depth Of Field: ", &gResources->eDOF);
            ImGui::Checkbox("Volumetric:     ", &gResources->eVolumetric);
            ImGui::Checkbox("Bloom:          ", &gResources->eBloom);
            ImGui::Checkbox("Tone Mapping:   ", &gResources->eToneMapping);
            ImGui::Checkbox("Vignette:       ", &gResources->eVignette);
            ImGui::Checkbox("Retro Grade:    ", &gResources->eRG);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            ImGui::Checkbox("Viewport", &gResources->eViewport);
            ImGui::Checkbox("Hierarchy", &gResources->eHierarchy);
            ImGui::Checkbox("Assets", &gResources->eAssets);
            ImGui::Checkbox("Keyframe Timeline", &gResources->eKeyframe);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::Text("FPS %.3f ms/frame (%.1f FPS)", ms, ImGui::GetIO().Framerate);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    Engine::RendererCommon::colorFBO->clear();
}

bool ImGuiLayer::OnKeyPress(Engine::KeyPressedEvent& e) {
    return false;
}

bool ImGuiLayer::OnMousePressed(Engine::MouseButtonPressedEvent& e) {
    return false;
}

bool ImGuiLayer::OnMouseMovedEvent(Engine::MouseMovedEvent& e) {
    return false;
}