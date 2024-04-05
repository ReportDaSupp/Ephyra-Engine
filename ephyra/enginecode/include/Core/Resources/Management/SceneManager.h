#pragma once
#include <json.hpp>
#include <fstream>
#include "Core/Resources/Management/ResourceManager.h"
#include "Core/Resources/Components/Components.h"
#include "Core/Resources/Utility/AssimpLoader.h"

namespace Engine
{

    void decomposeMtx(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
    {
        pos = m[3];
        for (int i = 0; i < 3; i++)
            scale[i] = glm::length(glm::vec3(m[i]));
        const glm::mat3 rotMtx(
            glm::vec3(m[0]) / scale[0],
            glm::vec3(m[1]) / scale[1],
            glm::vec3(m[2]) / scale[2]);
        rot = glm::quat_cast(rotMtx);
    }

    class SceneManager {
    public:
        static std::shared_ptr<SceneManager> getInstance() {
            static std::weak_ptr<SceneManager> weakInstance;
            std::shared_ptr<SceneManager> instance = weakInstance.lock();
            if (!instance) {
                instance = std::shared_ptr<SceneManager>(new SceneManager());
                weakInstance = instance;
            }
            return instance;
        }

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        void saveScene(const entt::registry& registry, const std::string& filepath) {
            nlohmann::json scene;

            auto view = registry.view<TagComponent>();

            for (auto entityID : view)
            {
                nlohmann::json entityJson;
                entityJson["id"] = entt::to_integral(entityID);

                // TransformComponent
                if (registry.all_of<TransformComponent>(entityID)) {
                    auto& entity = registry.get<TransformComponent>(entityID);
                    entityJson["TransformComponent"] = {
                        {"Translation", {entity.Translation.x, entity.Translation.y, entity.Translation.z}},
                        {"Euler",{entity.Euler.x, entity.Euler.y, entity.Euler.z}},
                        {"Scale",{entity.Scale.x, entity.Scale.y, entity.Scale.z}}
                    };
                }

                // MeshRendererComponent
                if (registry.all_of<MeshRendererComponent>(entityID)) {
                    auto& entity = registry.get<MeshRendererComponent>(entityID);
                    entityJson["MeshRendererComponent"] = {
                        {"FilePath", {entity.LoaderPath}}
                    };
                }

                // SpriteRendererComponent

                // CameraComponent
                
                // EmmissiveComponent
                if (registry.all_of<EmmissiveComponent>(entityID)) {
                    auto& entity = registry.get<EmmissiveComponent>(entityID);
                    entityJson["EmmissiveComponent"] = {
                        {"Position", {entity.Position.x, entity.Position.y, entity.Position.z}},
                        {"Color", {entity.Color.x, entity.Color.y, entity.Color.z}}
                    };
                }

                // RigidBodyComponent
                if (registry.all_of<RigidBodyComponent>(entityID)) {
                    auto& entity = registry.get<RigidBodyComponent>(entityID);
                    entityJson["RigidBodyComponent"] = {
                        {"BodyType", {entity.m_bodyType}}
                    };
                }

                // BoxColliderComponent
                if (registry.all_of<BoxColliderComponent>(entityID)) {
                    auto& entity = registry.get<BoxColliderComponent>(entityID);
                    entityJson["BoxColliderComponent"] = {
                        {"HalfExtents", {entity.halfExtents.x, entity.halfExtents.y, entity.halfExtents.z}}
                    };
                }

                // SphereColliderComponent
                if (registry.all_of<SphereColliderComponent>(entityID)) {
                    auto& entity = registry.get<SphereColliderComponent>(entityID);
                    entityJson["SphereColliderComponent"] = {
                        {"Radius", {entity.colliderRadius}}
                    };
                }

                // CapsuleColliderComponent
                if (registry.all_of<CapsuleColliderComponent>(entityID)) {
                    auto& entity = registry.get<CapsuleColliderComponent>(entityID);
                    entityJson["CapsuleColliderComponent"] = {
                        {"Radius", {entity.colliderRadius}},
                        {"Height", {entity.colliderHeight}}
                    };
                }

                // AudioSourceComponent
                
                // TagComponent
                if (registry.all_of<TagComponent>(entityID)) {
                    auto& entity = registry.get<TagComponent>(entityID);
                    entityJson["TagComponent"] = {
                        {"Tag", {entity.Tag}},
                        {"Type", {entity.Type}}
                    };
                }
                
                // StateComponent
                if (registry.all_of<StateComponent>(entityID)) {
                    auto& entity = registry.get<StateComponent>(entityID);
                    entityJson["StateComponent"] = {
                        {"State", {entity.State}}
                    };
                }

                scene["entities"].push_back(entityJson);
            };

            std::ofstream file("saves/" + filepath);
            file << scene.dump(4); // Pretty print with 4 spaces indent
        }

        void loadScene(entt::registry& registry, const std::string& filepath) {

            nlohmann::json scene;
            std::ifstream file("saves/" + filepath);

            // Check if file is open
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filepath << std::endl;
                return;
            }

            // Load the JSON file
            file >> scene;

            // Assuming you have a way to clear your current scene/registry
            registry.clear();

            // Iterate through the serialized entities
            if (scene.contains("entities")) {
                for (auto& entityJson : scene["entities"]) {
                    auto entityID = entityJson["id"].get<entt::entity>();
                    auto entity = registry.create(entityID);

                    // TransformComponent
                    if (entityJson.contains("TransformComponent")) {
                        auto& transComp = entityJson["TransformComponent"];
                        glm::vec3 T = glm::vec3(transComp["Translation"][0], transComp["Translation"][1], transComp["Translation"][2]);
                        glm::vec3 R = glm::vec3(transComp["Euler"][0], transComp["Euler"][1], transComp["Euler"][2]);
                        glm::vec3 S = glm::vec3(transComp["Scale"][0], transComp["Scale"][1], transComp["Scale"][2]);
                        registry.emplace<TransformComponent>(entity, T, R, S);
                    }

                    // MeshRendererComponent
                    if (entityJson.contains("MeshRendererComponent")) {
                        auto& meshRendererComp = entityJson["MeshRendererComponent"];
                        std::string temp = meshRendererComp["FilePath"][0];
                        std::string ID;
                        for (int i = temp.rfind('/') + 1; temp.length(); i++)
                        {
                            if (temp[i] != '.')
                                ID += temp[i];
                            else
                                break;
                        }
                        Engine::Loader::ASSIMPLoad(meshRendererComp["FilePath"][0], ID);
                        registry.emplace<MeshRendererComponent>(entity, gResources->getAsset<Geometry>(ID+"Geometry"), gResources->getAsset<Material>(ID + "Material"), temp);
                    }

                    // SpriteRendererComponent

                    // CameraComponent

                    // EmmissiveComponent
                    if (entityJson.contains("EmmissiveComponent")) {
                        auto& transComp = entityJson["EmmissiveComponent"];
                        glm::vec3 T = glm::vec3(transComp["Position"][0], transComp["Position"][1], transComp["Position"][2]);
                        glm::vec3 C = glm::vec3(transComp["Color"][0], transComp["Color"][1], transComp["Color"][2]);
                        registry.emplace<EmmissiveComponent>(entity, C, T);
                    }

                    // RigidBodyComponent
                    if (entityJson.contains("RigidBodyComponent")) {
                        auto& rigidBodyComp = entityJson["RigidBodyComponent"];
                        registry.emplace<RigidBodyComponent>(entity, entity, rigidBodyComp["BodyType"][0]);
                    }

                    // BoxColliderComponent
                    if (entityJson.contains("BoxColliderComponent")) {
                        auto& boxColliderComp = entityJson["BoxColliderComponent"];
                        glm::vec3 hExtents = glm::vec3(boxColliderComp["HalfExtents"][0], boxColliderComp["HalfExtents"][1], boxColliderComp["HalfExtents"][2]);
                        registry.emplace<BoxColliderComponent>(entity, entity, hExtents);
                    }

                    // SphereColliderComponent
                    if (entityJson.contains("SphereColliderComponent")) {
                        auto& sphereColliderComp = entityJson["SphereColliderComponent"];
                        registry.emplace<SphereColliderComponent>(entity, entity, sphereColliderComp["Radius"][0]);
                    }

                    // CapsuleColliderComponent
                    if (entityJson.contains("CapsuleColliderComponent")) {
                        auto& capsuleColliderComp = entityJson["CapsuleColliderComponent"];
                        registry.emplace<CapsuleColliderComponent>(entity, entity, capsuleColliderComp["Radius"][0], capsuleColliderComp["Height"][0]);
                    }

                    // AudioSourceComponent

                    // StateComponent
                    if (entityJson.contains("StateComponent")) {
                        auto& stateComp = entityJson["StateComponent"];
                        registry.emplace<StateComponent>(entity, stateComp["State"][0]);
                    }

                    // TagComponent
                    if (entityJson.contains("TagComponent")) {
                        auto& tagComp = entityJson["TagComponent"];
                        registry.emplace<TagComponent>(entity, tagComp["Tag"][0], (Engine::TagType)tagComp["Type"][0]);
                    }

                }
            }
        }


    private:
        SceneManager() { gResources = Engine::ResourceManager::getInstance(); }
        std::shared_ptr<ResourceManager> gResources;
    };
}