#pragma once
#include "Core/Resources/Management/ResourceManager.h"
#include "Core/Systems/Events/InputPoller.h"
#include "Core/Resources/Components/Components.h"
#include "Core/Resources/Utility/AssimpLoader.h"

class EngineLayer : public Engine::Layer {
private:

    std::shared_ptr<Engine::ResourceManager> gResources;

public:
    EngineLayer(const std::string& name = "EngineLayer")
        : Engine::Layer(name) { gResources = Engine::ResourceManager::getInstance(); }

    void OnAttach() override;
    void OnDetach() override;
    void bindHandlers(std::shared_ptr<Engine::Window> window, std::shared_ptr<Engine::InputPoller> poller) override {
        if (!gResources->m_window) gResources->m_window = window;
        if (!gResources->m_poller) gResources->m_poller = poller;
    };
    void OnUpdate(float timestep) override;
    void OnRender() override;
    bool OnKeyPress(Engine::KeyPressedEvent& e) override;
    bool OnMousePressed(Engine::MouseButtonPressedEvent& e) override;
    bool OnMouseMovedEvent(Engine::MouseMovedEvent& e) override;
};

void EngineLayer::OnAttach() {

    // Init Physics
    gResources->gPhysicsSystem.start();
    gResources->gPhysicsSystem.getPhysicsWorld()->setEventListener(&gResources->m_collisionListener);

    // Init 2D Audio
    /*gResources->gSoundPlayer.reset(new Engine::audioSystem2D);
    gResources->gSoundPlayer->start();
    gResources->gSoundPlayer->addSound("Background", "./assets/sounds/AmbientNoise.mp3");
    gResources->gSoundPlayer->setSoundLooping("Background", true);
    gResources->gSoundPlayer->playSound("Background");*/

    // Init 3D Renderer
    Engine::Renderer3D::init(49152, 49152, 16384);

    // Init Shader
    std::shared_ptr<Engine::Shader> PBRShader;
    PBRShader.reset(Engine::Shader::create("./assets/shaders/PBRShader.glsl"));
    gResources->addAsset("PBR", Engine::SceneAsset::Type::Shader, PBRShader);
    
    Engine::Renderer3D::initShader(gResources->getAsset<Engine::Shader>("PBR"));

    // Initialize Cameras
    gResources->Cameras["FirstPersonCamera"] = std::make_shared<Engine::FirstPersonCamera>();
    gResources->Cameras["FirstPersonCamera"]->attachWindow(gResources->m_window, gResources->m_poller);

    // Render Commands
    Engine::RendererCommon::actionCommand(gResources->glEnableBlend);
    Engine::RendererCommon::actionCommand(gResources->setBlendFunc);
    Engine::RendererCommon::actionCommand(gResources->setClearColourCommand);
    Engine::RendererCommon::actionCommand(gResources->glEnableDepthTest);

    // Bind Active Camera
    gResources->activeCamera = gResources->Cameras["FirstPersonCamera"];
    gResources->activeCamera->attachWindow(gResources->m_window, gResources->m_poller);

    // Get Camera Matricies
    gResources->m_view3D = gResources->activeCamera->getViewMatrix();
    gResources->m_projection3D = gResources->activeCamera->getProjectionMatrix();
    gResources->m_viewPos3D = gResources->activeCamera->getPosition();

    // Set Scene Wide Uniforms
    gResources->sWideUniforms3D["u_projection"] = std::pair<Engine::ShaderDataType, void*>(Engine::ShaderDataType::Mat4, static_cast<void*>(glm::value_ptr(gResources->m_projection3D)));
    gResources->sWideUniforms3D["u_view"] = std::pair<Engine::ShaderDataType, void*>(Engine::ShaderDataType::Mat4, static_cast<void*>(glm::value_ptr(gResources->m_view3D)));
    gResources->sWideUniforms3D["u_viewPos"] = std::pair<Engine::ShaderDataType, void*>(Engine::ShaderDataType::Float3, static_cast<void*>(glm::value_ptr(gResources->m_viewPos3D)));

    auto Floor = gResources->m_registry.create();
    gResources->m_registry.emplace<Engine::TagComponent>(Floor, "Floor", Engine::TagType::Render3D);
    gResources->m_registry.emplace<Engine::StateComponent>(Floor, true);
    gResources->m_registry.emplace<Engine::TransformComponent>(Floor, glm::vec3{ 0.f,-1.f,0.f }, glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 176.f, 1.f, 136.f });
    gResources->m_registry.emplace<Engine::MeshRendererComponent>(Floor, "./assets/models/Floor/Floor.obj", "Floor");
    gResources->m_registry.emplace<Engine::RigidBodyComponent>(Floor, Floor, rp3d::BodyType::STATIC);
    gResources->m_registry.emplace<Engine::BoxColliderComponent>(Floor, Floor, glm::vec3(88.f, 1.0f, 68.f));

}

void EngineLayer::OnDetach() {
    // Cleanup code here
}

void EngineLayer::OnUpdate(float timestep) {
    // Update logic for each frame

    // Update Physics
    if (gResources->ePhysics)
    {
        gResources->gPhysicsSystem.update(timestep);

        auto& rbView = gResources->m_registry.view<Engine::RigidBodyComponent>();
        for (auto& e : rbView)
        {
            auto& tc = gResources->m_registry.get<Engine::TransformComponent>(e);
            auto& rbc = gResources->m_registry.get<Engine::RigidBodyComponent>(e);

            auto& p = rbc.m_body->getTransform().getPosition();
            tc.Translation = glm::vec3(p.x, p.y, p.z);
            auto& o = rbc.m_body->getTransform().getOrientation();
            tc.Rotation = glm::quat(o.w, o.x, o.y, o.z);
            auto T = tc.Translation;
            glm::mat4 t = glm::translate(glm::mat4(1.0f), T);
            tc.Euler = glm::eulerAngles(tc.Rotation);
            auto R = tc.Rotation;
            glm::mat4 r = glm::mat4_cast(R);
            auto S = tc.Scale;
            glm::mat4 s = glm::scale(glm::mat4(1.0f), S);
            tc.Transform = t * r * s;
        }
    }

        // Update ViewPoint
    gResources->m_view3D = gResources->activeCamera->getViewMatrix();

        // Update Projection
    gResources->m_projection3D = gResources->activeCamera->getProjectionMatrix();

        // Update Camera Position
    gResources->m_viewPos3D = gResources->activeCamera->getPosition();

        // Call Render
    this->OnRender();

    if (!gResources->isGuiActive)
        gResources->activeCamera->update(timestep);
    
}

void EngineLayer::OnRender(){

    //Call Render Commands
    Engine::RendererCommon::actionCommand(gResources->setClearColourCommand);
    Engine::RendererCommon::actionCommand(gResources->clearCommand);
    Engine::RendererCommon::actionCommand(gResources->glDisableBlend);
    Engine::RendererCommon::actionCommand(gResources->glEnableDepthTest);

    Engine::RendererCommon::lightPos.clear();
    Engine::RendererCommon::lightColour.clear();

    auto& view1 = gResources->m_registry.view<Engine::EmmissiveComponent>();

    for (auto entity : view1)
    {
        glm::vec3 Position;
        bool vis = true;

        Position = gResources->m_registry.get<Engine::EmmissiveComponent>(entity).Position;

        if (gResources->m_registry.all_of<Engine::TransformComponent>(entity))
        {
            Position += gResources->m_registry.get<Engine::TransformComponent>(entity).Translation;
        }

        if (gResources->m_registry.all_of<Engine::StateComponent>(entity))
        {
           vis = gResources->m_registry.get<Engine::StateComponent>(entity).State;
        }

        if (vis)
        {
            Engine::RendererCommon::lightPos.push_back(Position);
            Engine::RendererCommon::lightColour.push_back(gResources->m_registry.get<Engine::EmmissiveComponent>(entity).Color);
        }
    }

    while (Engine::RendererCommon::lightPos.size() < 64)
    {
        Engine::RendererCommon::lightPos.push_back({0.f, 0.f, 0.f});
    }

    while (Engine::RendererCommon::lightColour.size() < 64)
    {
        Engine::RendererCommon::lightColour.push_back({ 0.f, 0.f, 0.f });
    }
   
    Engine::Renderer3D::begin(gResources->sWideUniforms3D);

    auto& view2 = gResources->m_registry.view<Engine::TransformComponent, Engine::MeshRendererComponent, Engine::StateComponent>();

    for (auto& entity : view2)
    {
        auto& mat = view2.get<Engine::MeshRendererComponent>(entity).Material;
        auto& geom = view2.get<Engine::MeshRendererComponent>(entity).Geometry;
        auto& trans = view2.get<Engine::TransformComponent>(entity);
        auto& vis = view2.get<Engine::StateComponent>(entity).State;
        if (vis) Engine::Renderer3D::submit(*geom, mat, trans);
    }

    bool enabledEffects[16] = { gResources->eDOF, gResources->eVolumetric, gResources->eBloom, gResources->eToneMapping, gResources->eVignette,gResources->eRG,1,1,1,1,1,1,1,1,1,1 };

    Engine::Renderer3D::end(enabledEffects);
}

bool EngineLayer::OnKeyPress(Engine::KeyPressedEvent& e) {
    e.handle(true);
    int keycode = e.getKeyCode();
    int scancode = glfwGetKeyScancode(keycode);
    int action = 1;
    int mod = 0;
    const char* key = glfwGetKeyName(keycode, scancode);
    Engine::Log::info("Key Pressed event - Key: {0}", key);
    if (0 <= key < 350)
        gResources->m_poller->processKeyPress(keycode, scancode, action, mod);
    return e.handled();
}

bool EngineLayer::OnMousePressed(Engine::MouseButtonPressedEvent& e){
    e.handle(true);
    auto button = e.getButton();
    Engine::Log::info("Mouse Button Pressed event: ({0})", button);
    gResources->m_poller->processMouseClick(button, 1, 0);
    return e.handled();
}

bool EngineLayer::OnMouseMovedEvent(Engine::MouseMovedEvent& e){
    e.handle(true);
    auto& position = e.getPos();
    Engine::Log::info("Mouse Moved event: ({0}, {1})", position.x, position.y);
    gResources->m_poller->processMouseMove(position.x, position.y);
    return e.handled();
}
