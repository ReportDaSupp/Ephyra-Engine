/**
 * \file BaseScene.h
 * \brief Contains the declaration of the BaseScene class.
 */
#pragma once

#include <glm/gtc/type_ptr.hpp>
#include "systems/importObj.h"
#include "rendering/shader.h"
#include "core/window.h"
#include "core/inputPoller.h"
#include "systems/camera.h"
#include "systems/cameraFPS.h"
#include "systems/cameraTP.h"
#include "rendering/renderer2D.h"
#include "rendering/renderer3D.h"

namespace Engine
{
    /**
     * \class BaseScene
     * \brief An Inheritable Base Class For Scene Creation.
     */
    class BaseScene
    {
    public:
        /**
         * \brief Constructs a BaseScene object.
         * \param window A shared pointer to the Window object.
         * \param poller A shared pointer to the InputPoller object.
         */
        BaseScene(std::shared_ptr<Window> window, std::shared_ptr<InputPoller> poller) : m_window(window), m_poller(poller) {};

        /**
         * \brief Destroys the BaseScene object.
         */
        virtual ~BaseScene() = default;

        /**
         * \brief Updates the scene.
         * \param deltaTime The time since the last update.
         * \return True if the update was successful, false otherwise.
         */
        virtual bool OnUpdate(float deltaTime) { return true; };

        /**
         * \brief Renders the scene.
         */
        virtual void OnRender() {};

        /**
         * \brief Renders the ImGui elements of the scene.
         */
        virtual void OnImGuiRender() {};

        /**
         * \brief Attaches a camera to the scene.
         * \param cameraName The name of the camera to attach.
         */
        virtual void AttachCamera(const std::string& cameraName) {};

        /**
         * \brief Checks if the scene is in fullscreen mode.
         * \return True if the scene is in fullscreen mode, false otherwise.
         */
        virtual bool isFullscreen() { return 0; };

    protected:
        std::unordered_map<std::string, std::shared_ptr<Shader>> Shaders; /**< Unordered Map of Scene Shaders */
        std::unordered_map<std::string, std::shared_ptr<Texture>> Textures; /**< Unordered Map of Scene Textures */
        std::unordered_map<std::string, std::shared_ptr<SubTexture>> SubTextures; /**< Unordered Map of Scene SubTextures */
        std::unordered_map<std::string, std::shared_ptr<ImportObj>> ImportedObjects; /**< Unordered Map of Scene ImportedGeometry */
        std::unordered_map<std::string, Geometry> Geometries; /**< Unordered Map of Scene Geometry, Non-Shared Pointer */
        std::unordered_map<std::string, Quad> Quads; /**< Unordered Map of Scene Quads, Non-Shared Pointer */
        std::unordered_map<std::string, std::shared_ptr<Material>> Materials; /**< Unordered Map of Scene Materials */
        std::unordered_map<std::string, std::shared_ptr<Camera>> Cameras; /**< Unordered Map of Scene Cameras */
        std::shared_ptr<Camera> activeCamera; /**< Current ActiveCamera */

        std::shared_ptr<Window> m_window; /**< Current Window Pointer Passed from SceneManager */
        std::shared_ptr<InputPoller> m_poller; /**< Current Input Poller Pointer Passed from SceneManager */

        SceneWideUniforms sWideUniforms2D; /**< Data Struct For Scene Wide Uniforms 3D, Non-Shared Pointer as Intrinsically Linked To This Scene */
        SceneWideUniforms sWideUniforms3D; /**< Data Struct For Scene Wide Uniforms 2D, Non-Shared Pointer as Intrinsically Linked To This Scene */
    };
}
