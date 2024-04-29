#pragma once

#include "Ephyra.h"
#include "Core/Systems/Utility/PhysicsSystem.h"
#include "Core/Rendering/API/Global/RenderCommands.h"
#include "Core/Rendering/API/Global/RendererCommon.h"

#include <memory>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <src/entt/entt.hpp>
#include <unordered_map>

namespace Engine
{

    struct SceneAsset {
        enum class Type {
            Undefined = 0,
            Transform,
            Quad,
            Shader,
            Texture,
            SubTexture,
            Material,
            Geometry
        };

        std::string id; // Unique identifier for the asset
        Type type;
        std::shared_ptr<void> asset; // Point to the actual asset, using void for generality

        SceneAsset(std::string id, Type type, std::shared_ptr<void> asset)
            : id(std::move(id)), type(type), asset(std::move(asset)) {}
    };

    class ResourceManager {
    public:
        static std::shared_ptr<ResourceManager> getInstance() {
            static std::weak_ptr<ResourceManager> weakInstance;
            std::shared_ptr<ResourceManager> instance = weakInstance.lock();
            if (!instance) {
                instance = std::shared_ptr<ResourceManager>(new ResourceManager());
                weakInstance = instance;
            }
            return instance;
        }

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;

        template<typename T>
        std::shared_ptr<T> addAsset(const std::string& id, SceneAsset::Type type, std::shared_ptr<T> asset) {
            loadedAssets.emplace_back(id, type, std::static_pointer_cast<void>(asset));
            return asset;
        }

        template<typename T>
        std::shared_ptr<T> getAsset(const std::string& id) {
            for (auto& asset : loadedAssets) {
                if (asset.id == id) {
                    return std::static_pointer_cast<T>(asset.asset);
                }
            }
            return nullptr; // Asset not found
        }

        std::vector<SceneAsset> getAllOf(const SceneAsset::Type type) {
            std::vector<SceneAsset> temp;
            for (auto& asset : loadedAssets) {
                if (asset.type == type) {
                    temp.emplace_back(asset);
                }
            }
            return temp;
        }

        std::vector<SceneAsset> getAll() {
            return loadedAssets;
        }

        // Global Functionality
        
            // Window Management
        std::shared_ptr<Engine::Window> m_window; /**< Current Window Pointer Passed from SceneManager */
        std::shared_ptr<Engine::InputPoller> m_poller; /**< Current Input Poller Pointer Passed from SceneManager */

            // Render Commands
        std::shared_ptr<Engine::RenderCommand> clearCommand; /**< The clear render command. */
        std::shared_ptr<Engine::RenderCommand> setClearColourCommand; /**< The set clear color render command. */
        std::shared_ptr<Engine::RenderCommand> glEnableBlend; /**< The glEnable blend render command. */
        std::shared_ptr<Engine::RenderCommand> glDisableBlend; /**< The glDisable blend render command. */
        std::shared_ptr<Engine::RenderCommand> glEnableDepthTest; /**< The glEnable depth test render command. */
        std::shared_ptr<Engine::RenderCommand> glDisableDepthTest; /**< The glDisable depth test render command. */
        std::shared_ptr<Engine::RenderCommand> setBlendFunc; /**< The set blend function render command. */

        // Scene Data
        std::unordered_map<std::string, std::shared_ptr<Engine::Camera>> Cameras; /**< Unordered Map of Scene Cameras */
        std::shared_ptr<Engine::Camera> activeCamera; /**< Current ActiveCamera */

        Engine::SceneWideUniforms sWideUniforms2D;
        Engine::SceneWideUniforms sWideUniforms3D; /**< Data Struct For Scene Wide Uniforms 3D, Non-Shared Pointer as Intrinsically Linked To This Scene */

        std::shared_ptr<Engine::TextureAtlas> texAtlas2D;

        glm::mat4 m_view3D; /**< The 3D view matrix. */
        glm::mat4 m_projection3D; /**< The 3D projection matrix. */
        glm::vec3 m_viewPos3D; /**< The 3D view position. */
        glm::mat4 m_view2D;
        glm::mat4 m_projection2D;

        std::unordered_map<std::string, std::vector<std::string>> FPToIDs;
        std::unordered_map<std::string, std::vector<std::string>> IDToMeshNames;
        uint32_t fileCount = 0;

        // GUI Layer
        bool isGuiActive = true;
        bool eDOF = false;
        bool eVolumetric = true;
        bool eBloom = true;
        bool eVignette = false;
        bool eToneMapping = true;
        bool eRG = false;
        bool ePhysics = true;

        bool eViewport = true;
        bool eTextureViewer = false;
        bool eHierarchy = true;
        bool eAssets= true;
        bool eKeyframe = true;
        bool eFileDialog = false;

        bool eIntroMessage = true;

        float currentTimeKey = 0.f;

        std::shared_ptr<Texture> imGuiWelcomeImage;
        std::shared_ptr<Texture> texViewerImage;


        std::string selectedObject;

        // Registry
        entt::registry m_registry;

        // React3D
        friend class CollisionListener;
        PhysicsSystem gPhysicsSystem;
        CollisionListener m_collisionListener;

        // Audio 2D
        std::shared_ptr<audioSystem2D> gSoundPlayer;

    private:

        std::vector<SceneAsset> loadedAssets;
        

        ResourceManager()
        {
            clearCommand.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::clearColourAndDepthBuffer));
            setClearColourCommand.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::setClearColour, 0.05f, 0.05f, 0.05f, 1.0f));
            glEnableBlend.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::Enable, GL_BLEND));
            glDisableBlend.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::Disable, GL_BLEND));
            glEnableDepthTest.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::Enable, GL_DEPTH_TEST));
            glDisableDepthTest.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::Disable, GL_DEPTH_TEST));
            setBlendFunc.reset(Engine::RenderCommandFactory::createCommand(Engine::RenderCommand::Commands::setBlendFunc));
        }
    };

}

// Include guards to prevent multiple inclusion
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

extern std::shared_ptr<Engine::ResourceManager> gResourceManager;

#endif