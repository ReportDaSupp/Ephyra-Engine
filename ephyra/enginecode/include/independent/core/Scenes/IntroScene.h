/**
 * \file IntroScene.h
 * \brief Contains the declaration of the IntroScene class, Based On The BaseScene.
 */
#pragma once
#include "BaseScene.h"

namespace Ephyra
{
	/**
	* \class IntroScene.h
	* \brief Declaration of IntroScene class, Based On BaseScene.
	*/
	class IntroScene : public BaseScene
	{
	public:
		/**
		 * \brief Constructs a IntroScene object.
		 * \param window A shared pointer to the Window object.
		 * \param poller A shared pointer to the InputPoller object.
		 */
		IntroScene(std::shared_ptr<Window> window, std::shared_ptr<InputPoller> poller);
		~IntroScene();

		bool OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

		void AttachCamera(const std::string& cameraName) override;

		bool isFullscreen() override { return m_fullScreen; };

	private:
		void Init2DRenderer();
		void Init3DRenderer();
		void InitImGui();

		void InitShaders();
		void InitCamera(std::string camName, std::shared_ptr<Camera> camera);
		void swapCam();

		void InitSprites();
		void InitModels();
		void InitGeometry();
		void InitLighting();
		void InitTextures();

		void InitRenderCommands();

		void Render2D();
		void Render3D();

		glm::mat4 modelTransforms[125]; /**< The model transforms. */
		glm::mat4 cameraTransform; /**< The camera transform. */
		glm::mat4 floorTransform; /**< The floor transform. */
		glm::mat4 skyBoxTransform; /**< The skybox transform. */

	protected:
		std::shared_ptr<TextureAtlas> texAtlas2D; /**< The 2D texture atlas. */

		glm::mat4 m_view2D; /**< The 2D view matrix. */
		glm::mat4 m_projection2D; /**< The 2D projection matrix. */

		glm::mat4 m_view3D; /**< The 3D view matrix. */
		glm::mat4 m_projection3D; /**< The 3D projection matrix. */
		glm::vec3 m_viewPos3D; /**< The 3D view position. */

	private:
		std::shared_ptr<RenderCommand> clearCommand; /**< The clear render command. */
		std::shared_ptr<RenderCommand> setClearColourCommand; /**< The set clear color render command. */
		std::shared_ptr<RenderCommand> glEnableBlend; /**< The glEnable blend render command. */
		std::shared_ptr<RenderCommand> glDisableBlend; /**< The glDisable blend render command. */
		std::shared_ptr<RenderCommand> glEnableDepthTest; /**< The glEnable depth test render command. */
		std::shared_ptr<RenderCommand> glDisableDepthTest; /**< The glDisable depth test render command. */
		std::shared_ptr<RenderCommand> setBlendFunc; /**< The set blend function render command. */

		bool drawUI = true; /**< Bool indicating whether to draw UI elements. */
		int cubesDrawn = 5; /**< The number of cubes to draw. */
		glm::vec3 albedo = { 1, 1, 1 }; /**< The albedo color. */
		glm::vec3 SuzanneValues = { 1.0, 0.4, 0.2 }; /**< The Suzanne values. */
		glm::vec3 FloorValues = { 0.35, 0.25, 0.2 }; /**< The floor values. */

		const char* localAssetPath = "./assets/models/Suzanne.obj";
		const char* localGrimePath = "assets/textures/Grime.png";

		bool m_fullScreen = false; /**< Bool indicating whether the scene is in fullscreen mode. */
		bool dispControls = false; /**< Bool indicating whether to display controls. */
	};
}