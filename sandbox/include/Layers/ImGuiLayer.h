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

    this->OnRender();
}

void ImGuiLayer::OnRender() {

    //if (ImGui::BeginMainMenuBar())
    //{
    //    ImGui::TextColored(TitleColor, "Untitled Horror Game ");

    //    ImGui::Separator();
    //     
    //    if (ImGui::BeginMenu("Loaded Data"))
    //    {
    //        if (gResources->isGuiActive)
    //        {
    //            for (auto temp : gResources->getAll())
    //            {
    //                switch (temp.type)
    //                {
    //                    case Engine::SceneAsset::Type::Undefined:
    //                    {
    //                        ImGui::Text("Undefined: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::Transform:
    //                    {
    //                        ImGui::Text("Transform: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::Quad:
    //                    {
    //                        ImGui::Text("Quad: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::Shader:
    //                    {
    //                        ImGui::Text("Shader: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::Texture:
    //                    {
    //                        ImGui::Text("Texture: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::SubTexture:
    //                    {
    //                        ImGui::Text("SubTexture: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::Material:
    //                    {
    //                        ImGui::Text("Material: %s", temp.id);
    //                        break;
    //                    }
    //                    case Engine::SceneAsset::Type::Geometry:
    //                    {
    //                        ImGui::Text("Geometry: %s", temp.id);
    //                        break;
    //                    }
    //                }

    //                
    //            }
    //        }
    //        ImGui::EndMenu();
    //    }


    //    

    //    ImGui::Separator();

    //    if (gResources->ePhysics)
    //    {
    //        if (ImGui::Button("Pause"))
    //        {
    //            gResources->ePhysics = false;
    //        }
    //    }
    //    else
    //    {
    //        if (ImGui::Button("Play"))
    //        {
    //            gResources->ePhysics = true;
    //            auto& view = gResources->m_registry.view<Engine::TransformComponent>();
    //            for (auto& asset : view)
    //            {
    //                if (gResources->m_registry.all_of<Engine::RigidBodyComponent>(asset))
    //                {
    //                    auto& rbc = gResources->m_registry.get<Engine::RigidBodyComponent>(asset);
    //                    auto& transformation = gResources->m_registry.get<Engine::TransformComponent>(asset);
    //                    reactphysics3d::Transform transform;
    //                    transform.setPosition({ transformation.Translation.x,  transformation.Translation.y,  transformation.Translation.z });
    //                    transform.setOrientation({ transformation.Rotation.x, transformation.Rotation.y, transformation.Rotation.z, transformation.Rotation.w });
    //                    rbc.m_body->setTransform(transform);
    //                }
    //            }
    //        }
    //    }

    //    ImGui::EndMainMenuBar();
    //}

    Engine::RendererCommon::colorFBO->unbind();

    float ms = 1000.0f / ImGui::GetIO().Framerate;

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGuiWindowFlags windowFlags = 0;
    static bool showDecoration = false;
    if (!showDecoration)
        windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

    if (gResources->eHierarchy)
    {
        if (ImGui::Begin("Scene Hierarchy", &gResources->eHierarchy, windowFlags))
        {
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Add Asset"))
            {
                std::string RenderTypes[] = { "Render3D", "Render2D", "Light"};
                static std::string RenderType = "Not Selected";

                if (ImGui::BeginCombo("Render Type", RenderType.c_str())) // The second parameter is the label previewed before opening the combo.
                {
                    for (auto type : RenderTypes)
                    {
                        bool is_selected = (RenderType == type); // You can store your selection however you want, outside or inside your objects
                        if (ImGui::Selectable(type.c_str(), is_selected))
                        {
                            RenderType = type;
                        }
                        if (is_selected)
                        {
                           ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                        }
                    }
                    ImGui::EndCombo();
                }
                if (RenderTypes[0] == RenderType)
                {   
                    static char str1[128] = "";
                    ImGui::InputTextWithHint("Object Name", "enter name here", str1, IM_ARRAYSIZE(str1));

                    static std::string inputMaterial = "Not Selected";
                    if (ImGui::BeginCombo("Material", inputMaterial.c_str())) // The second parameter is the label previewed before opening the combo.
                    {

                        for (auto& mat : gResources->getAllOf(Engine::SceneAsset::Type::Material))
                        {
                            bool is_selected = (inputMaterial == mat.id); // You can store your selection however you want, outside or inside your objects
                            if (ImGui::Selectable(mat.id.c_str(), is_selected))
                            {
                                inputMaterial = mat.id;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                            }
                        }
                        ImGui::EndCombo();
                    }

                    static std::string inputGeometry = "Not Selected";
                    if (ImGui::BeginCombo("Geometry", inputGeometry.c_str())) // The second parameter is the label previewed before opening the combo.
                    {
                        for (auto& geom : gResources->getAllOf(Engine::SceneAsset::Type::Geometry))
                        {
                            bool is_selected = (inputGeometry == geom.id); // You can store your selection however you want, outside or inside your objects
                            if (ImGui::Selectable(geom.id.c_str(), is_selected))
                            {
                                inputGeometry = geom.id;
                            }
                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                            }
                        }
                        ImGui::EndCombo();
                    }

                    static glm::vec3 inputTranslation = {0.f, 0.f, 0.f};
                    static glm::vec3 inputRotation = { 0.f, 0.f, 0.f };
                    static glm::vec3 inputScale = { 1.f, 1.f, 1.f };

                    ImGui::DragFloat3("Translation: ", &inputTranslation.x);
                    ImGui::DragFloat3("Rotation: ", &inputRotation.x);
                    ImGui::DragFloat3("Scale: ", &inputScale.x);

                    static bool Emmissive = false;
                    ImGui::Checkbox("Emmissive", &Emmissive);

                    if (inputMaterial != "Not Selected" && inputGeometry != "Not Selected")
                    {
                        if (ImGui::Button("Instance Asset"))
                        {
                            auto newAsset = gResources->m_registry.create();
                            gResources->m_registry.emplace<Engine::TagComponent>(newAsset, (std::string)(str1), Engine::TagType::Render3D);
                            gResources->m_registry.emplace<Engine::StateComponent>(newAsset, true);
                            gResources->m_registry.emplace<Engine::TransformComponent>(newAsset, inputTranslation, inputRotation, inputScale);
                            //gResources->m_registry.emplace<Engine::MeshRendererComponent>(newAsset, (gResources->getAsset<Engine::Geometry>(inputGeometry), (gResources->getAsset<Engine::Material>(inputMaterial)));
                            //gResources->m_registry.emplace<Engine::GeometryComponent>(newAsset, (gResources->getAsset<Engine::Geometry>(inputGeometry)));

                            if (Emmissive)
                                gResources->m_registry.emplace<Engine::EmmissiveComponent>(newAsset, glm::vec3({ 30.f,30.f,30.f }), glm::vec3{ 0.f,0.f,0.f });

                            inputMaterial = "Not Selected";
                            inputGeometry = "Not Selected";
                            RenderType = "Not Selected";
                            
                            inputTranslation = { 0.f, 0.f, 0.f };
                            inputRotation = { 0.f, 0.f, 0.f };
                            inputScale = { 1.f, 1.f, 1.f };
                            
                            Emmissive = false;
                        }
                    }
                    
                }
                else if (RenderTypes[1] == RenderType)
                {

                }
                else if (RenderTypes[2] == RenderType)
                {
                    static char str1[128] = "";
                    ImGui::InputTextWithHint("Light Name", "enter name here", str1, IM_ARRAYSIZE(str1));

                    static glm::vec3 iPosition = { 0.f, 0.f, 0.f };
                    static glm::vec3 iColor = { 1.f, 1.f, 1.f };

                    ImGui::DragFloat3("Position: ", &iPosition.x, 0.1f);
                    ImGui::DragFloat3("Color: ", &iColor.x, 0.1f);

                        if (ImGui::Button("Instance Asset"))
                        {
                            auto newAsset = gResources->m_registry.create();
                            gResources->m_registry.emplace<Engine::TagComponent>(newAsset, (std::string)(str1), Engine::TagType::Light);
                            gResources->m_registry.emplace<Engine::EmmissiveComponent>(newAsset, iColor, iPosition);
                            gResources->m_registry.emplace<Engine::StateComponent>(newAsset, true);

                            RenderType = "Not Selected";

                            iPosition = { 0.f, 0.f, 0.f };
                            iColor = { 1.f, 1.f, 1.f };
                        }
                }
                else
                {

                }
            }
            ImGui::Separator();
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
    if (gResources->eAssets)
    {
        if (ImGui::Begin("Assets", &gResources->eAssets, windowFlags))
        {
            for (auto temp : gResources->getAll())
            {
                switch (temp.type)
                {
                    case Engine::SceneAsset::Type::Undefined:
                    {
                        ImGui::Text("Undefined: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Transform:
                    {
                        ImGui::Text("Transform: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Quad:
                    {
                        ImGui::Text("Quad: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Shader:
                    {
                        ImGui::Text("Shader: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Texture:
                    {
                        ImGui::Text("Texture: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::SubTexture:
                    {
                        ImGui::Text("SubTexture: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Material:
                    {
                        ImGui::Text("Material: %s", temp.id);
                        break;
                    }
                    case Engine::SceneAsset::Type::Geometry:
                    {
                        ImGui::Text("Geometry: %s", temp.id);
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