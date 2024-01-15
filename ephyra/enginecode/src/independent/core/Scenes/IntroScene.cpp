#include "engine_pch.h"
#include "Core/Scenes/IntroScene.h"
#include "platform/GLFW/ImGuiHelper.h"
#include <GLFW/glfw3.h>


namespace Engine
{

	IntroScene::IntroScene(std::shared_ptr<Window> window, std::shared_ptr<InputPoller> poller) : BaseScene(window, poller)
	{
		this->InitShaders();
		this->Init2DRenderer();
		this->Init3DRenderer();
		this->InitRenderCommands();
		this->InitImGui();

		RendererCommon::actionCommand(glEnableBlend);
		RendererCommon::actionCommand(setBlendFunc);
		RendererCommon::actionCommand(setClearColourCommand);
		RendererCommon::actionCommand(glEnableDepthTest);
	}

	IntroScene::~IntroScene()
	{

	}

	bool IntroScene::OnUpdate(float deltaTime)
	{
		glDisable(GL_SCISSOR_TEST);
		RendererCommon::actionCommand(clearCommand);

		GLFWwindow* window = (GLFWwindow*)m_window->getNativeWindow();
		int posx, posy, width, height;

		glfwGetWindowSize(window, &width, &height);

		float viewportWidth = ((ASPECT_RATIO.x/ASPECT_RATIO.y) * (float)height);
		float viewportHeight = (float)height;
		posx = (width - viewportWidth) / 2;
		posy = (height - viewportHeight) / 2;

		glViewport(posx, posy, viewportWidth, viewportHeight);
		glEnable(GL_SCISSOR_TEST);
		glScissor(posx, posy, viewportWidth, viewportHeight);

		

		// Update ViewPoint
		m_view3D = activeCamera->getViewMatrix();

		// Update Projection
		m_projection3D = activeCamera->getProjectionMatrix();

		// Update Camera Position
		m_viewPos3D = activeCamera->getPosition();
		
		m_projection2D = glm::ortho(0.f, static_cast<float>(m_window->getWidth()), static_cast<float>(m_window->getHeight()), 0.f);

		cameraTransform = glm::mat4(1.0);
		cameraTransform = glm::translate(cameraTransform, Cameras["ThirdPersonCamera"]->getCentralPosition());
		cameraTransform = glm::scale(cameraTransform, glm::vec3(0.3f, 0.3f, 0.3f));
		
		

		this->OnRender();

		// Update Display
		m_window->onUpdate(deltaTime);

		activeCamera->update(deltaTime);

		if (!activeCamera->isRunning())
		{
			return false;
		}

		return true;
	}

	void IntroScene::OnRender()
	{

		OnImGuiRender();

		// Clear Screen
		RendererCommon::actionCommand(setClearColourCommand);
		RendererCommon::actionCommand(clearCommand);

		// Enable to allow 3D draw's for UI
		RendererCommon::actionCommand(glDisableBlend);
		RendererCommon::actionCommand(glEnableDepthTest);

		this->Render3D();

		if (drawUI)
		{
			// Disable to allow 2D draw's for UI
			RendererCommon::actionCommand(glDisableDepthTest);
			RendererCommon::actionCommand(glEnableBlend);

			this->Render2D();
		}
		RendererCommon::actionCommand(glDisableBlend);

		ImGuiHelper::end();
		
	}

	void IntroScene::OnImGuiRender()
	{
		ImGuiHelper::begin();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (activeCamera->getMouseEnabled())
		{
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
		else
		{
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}

		ImGui::Begin("Scene Modifiers:");
		ImGui::Text("Press Alt / Tab To Enable or Disable Cursor");
		ImGui::Checkbox("Draw Ui", &drawUI);
		ImGui::Checkbox("Fullscreen", &m_fullScreen);
		ImGui::Checkbox("Display Controls", &dispControls);
		if (ImGui::Button("Swap Camera - FPS x TP")) swapCam();
		ImGui::InputFloat2("Aspect Ratio", &ASPECT_RATIO.x);
		ImGui::DragInt("Number Of Models", &cubesDrawn, 1, 0, 125);
		ImGui::DragFloat3("Albedo", &albedo.x, 0.05, 0, 1);
		ImGui::DragFloat3("Suzanne Values", &SuzanneValues.x, 0.05, 0, 1);
		ImGui::DragFloat3("Sphere Values", &SphereValues.x, 0.05, 0, 1);
		ImGui::DragFloat3("Floor Values", &FloorValues.x, 0.05, 0, 1);
		ImGui::DragFloat3("Light 1 Pos", &RendererCommon::lightPos.at(0).x, 0.1);
		ImGui::DragFloat3("Light 1 Colour", &RendererCommon::lightColour.at(0).x, 0.05, 0, 256);
		ImGui::DragFloat3("Light 2 Pos", &RendererCommon::lightPos.at(1).x, 0.1);
		ImGui::DragFloat3("Light 2 Colour", &RendererCommon::lightColour.at(1).x, 0.05, 0, 256);
		ImGui::DragFloat3("Light 3 Pos", &RendererCommon::lightPos.at(2).x, 0.1);
		ImGui::DragFloat3("Light 3 Colour", &RendererCommon::lightColour.at(2).x, 0.05, 0, 256);
		ImGui::DragFloat3("Light 4 Pos", &RendererCommon::lightPos.at(3).x, 0.1);
		ImGui::DragFloat3("Light 4 Colour", &RendererCommon::lightColour.at(3).x, 0.05, 0, 256);

		if (dispControls && !activeCamera->getMouseEnabled())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Controls:");
			ImGui::BulletText("W,A,S,D     : Horizontal Movement");
			ImGui::BulletText("Space, Ctrl : Vertical Movement");
			ImGui::BulletText("Shift       : Cam Shake & Increased Move Speed");
			ImGui::BulletText("Scroll      : Zoom In & Out");
			ImGui::BulletText("Tab / Alt   : Enable / Disable Cursor");
			ImGui::EndTooltip();
		}

		ImGui::End();

	}

	void IntroScene::AttachCamera(const std::string& cameraName)
	{
		activeCamera = Cameras[cameraName];
		activeCamera->attachWindow(m_window, m_poller);
		m_view3D = activeCamera->getViewMatrix();
		m_projection3D = activeCamera->getProjectionMatrix();
		m_viewPos3D = activeCamera->getPosition();

		sWideUniforms3D["u_projection"] = std::pair<ShaderDataType, void*>(ShaderDataType::Mat4, static_cast<void*>(glm::value_ptr(m_projection3D)));
		sWideUniforms3D["u_view"] = std::pair<ShaderDataType, void*>(ShaderDataType::Mat4, static_cast<void*>(glm::value_ptr(m_view3D)));
		sWideUniforms3D["u_viewPos"] = std::pair<ShaderDataType, void*>(ShaderDataType::Float3, static_cast<void*>(glm::value_ptr(m_viewPos3D)));
	}

	void IntroScene::Init2DRenderer()
	{
		this->InitSprites();

		m_view2D = glm::mat4(1.f);
		m_projection2D = glm::ortho(0.f, static_cast<float>(m_window->getWidth()), static_cast<float>(m_window->getHeight()), 0.f);

		sWideUniforms2D["u_projection"] = std::pair<ShaderDataType, void*>(ShaderDataType::Mat4, static_cast<void*>(glm::value_ptr(m_projection2D)));
		sWideUniforms2D["u_view"] = std::pair<ShaderDataType, void*>(ShaderDataType::Mat4, static_cast<void*>(glm::value_ptr(m_view2D)));
		
		Renderer2D::init();
	}

	void IntroScene::Init3DRenderer()
	{
		this->InitModels();
		this->InitTextures();
		this->InitCamera("FirstPersonCamera", std::make_shared<FirstPersonCamera>());
		this->InitCamera("ThirdPersonCamera", std::make_shared<ThirdPersonCamera>(glm::vec3{0,0,7}));
		this->AttachCamera(std::string("FirstPersonCamera"));
		this->InitLighting();

		Renderer3D::init(16384, 16384, 16384);
		this->InitGeometry();
		Renderer3D::initShader(Shaders["PBR"]);
	}

	void IntroScene::InitImGui()
	{
		ImGuiHelper::init();
	}

	void IntroScene::InitShaders()
	{
		// Initi Shaders
		std::shared_ptr<Shader> PBRShader;
		PBRShader.reset(Shader::create("./assets/shaders/PBRShader.glsl"));
		Shaders["PBR"] = PBRShader;
	}

	void IntroScene::InitCamera(std::string camName, std::shared_ptr<Camera> cam)
	{
		Cameras[camName] = cam;
		Cameras[camName]->attachWindow(m_window, m_poller);
	}

	void IntroScene::swapCam()
	{
		if (activeCamera == Cameras["FirstPersonCamera"])
		{
			AttachCamera("ThirdPersonCamera");
		}
		else
		{
			AttachCamera("FirstPersonCamera");
		}
	}

	void IntroScene::InitSprites()
	{
		// Texture Atlas Population For UI Elements
		texAtlas2D = std::make_shared<TextureAtlas>();

		SubTextures["Hotbar"] = std::make_shared<SubTexture>(texAtlas2D->getBaseTexture(), glm::vec2(0.f, 0.f), glm::vec2(1.f, 1.f));
		SubTextures["Crosshair"] = std::make_shared<SubTexture>(texAtlas2D->getBaseTexture(), glm::vec2(0.f, 0.f), glm::vec2(1.f, 1.f));
		SubTextures["Item"] = std::make_shared<SubTexture>(texAtlas2D->getBaseTexture(), glm::vec2(0.f, 0.f), glm::vec2(1.f, 1.f));

		texAtlas2D->add("assets/textures/itemText1.png", *SubTextures["Item"]);
		texAtlas2D->add("assets/textures/crosshairTexture.png", *SubTextures["Crosshair"]);
		texAtlas2D->add("assets/textures/HotbarTex.png", *SubTextures["Hotbar"]);

		// Define Quads for Scene
		Quads["Crosshair"] = Quads["Crosshair"].createCentreHalfExtents({m_window->getWidth() / 2, m_window->getHeight() / 2}, {25.f, 25.f});
		Quads["HealthBar"] = Quads["HealthBar"].createCentreHalfExtents({ 95.f, 30.f }, { 85.f, 10.f });

		for (int i = 1; i < 9; i++)
		{
			const std::string& temp = ("Hotbar " + std::to_string(i));
			Quads[temp] = Quads[temp].createCentreHalfExtents({ (((m_window->getWidth() / 8)) * i) - (m_window->getWidth() / 16), m_window->getHeight() - (m_window->getWidth() / 8) / 2 }, { (m_window->getWidth() / 8 / 2), (m_window->getWidth() / 8) / 2 });
		}
	}

	void IntroScene::InitModels()
	{
		// Import Scene Obj's
		ImportedObjects["Suzanne"] = std::make_shared<ImportObj>("./assets/models/Suzanne.obj");
		ImportedObjects["Plane"] = std::make_shared<ImportObj>("./assets/models/Cube.obj");
		ImportedObjects["Sphere"] = std::make_shared<ImportObj>("./assets/models/Sphere.obj");

		// Create Models
		modelTransforms[0] = glm::translate(glm::mat4(1.0f), glm::vec3(-3.f, 0.01f, -3.0f));

		cameraTransform = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
		cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 0.0f, 0.0f));

		floorTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, 0.0f));
		floorTransform = glm::scale(floorTransform, glm::vec3(40.f, 0.01f, 40.f));

		for (int y = 0; y < 5; y++)
		{
			for (int z = 0; z < 5; z++)
			{
				for (int x = 0; x < 5; x++)
				{

					modelTransforms[x + z * 5 + y * 5 * 5] = glm::translate(modelTransforms[0], glm::vec3(x * 2.f, y * 2.f, z * 2.f));
				}
			}
		}
	}

	void IntroScene::InitGeometry()
	{
		// Define Geometry
		Geometries["Suzanne"] = *std::make_shared<Geometry>();
		Geometries["Sphere"] = *std::make_shared<Geometry>();
		Geometries["Plane"] = *std::make_shared<Geometry>();

		if (!Renderer3D::addGeometry(ImportedObjects["Suzanne"]->getVertices(), ImportedObjects["Suzanne"]->getIndices(), Geometries["Suzanne"])) Log::error("Couldn't Add Geometry!");
		if (!Renderer3D::addGeometry(ImportedObjects["Sphere"]->getVertices(), ImportedObjects["Sphere"]->getIndices(), Geometries["Sphere"])) Log::error("Couldn't Add Geometry!");
		if (!Renderer3D::addGeometry(ImportedObjects["Plane"]->getVertices(), ImportedObjects["Plane"]->getIndices(), Geometries["Plane"])) Log::error("Couldn't Add Geometry!");
	}

	void IntroScene::InitLighting()
	{
		// Initialize Lights
		RendererCommon::lightPos.push_back(glm::vec3{ 1.f, 0.f, 1.f });
		RendererCommon::lightPos.push_back(glm::vec3{ -3.f, 6.f, -6.f });
		RendererCommon::lightPos.push_back(glm::vec3{ 4.f, 0.f, 0.f });
		RendererCommon::lightPos.push_back(glm::vec3{ -6.f, 3.f, 4.f });

		RendererCommon::lightColour.push_back(glm::vec3{ 52.f, 0.3f, 0.3f });
		RendererCommon::lightColour.push_back(glm::vec3{ 0.3f, 52.f, 0.3f });
		RendererCommon::lightColour.push_back(glm::vec3{ 0.3f, 0.3f, 52.f });
		RendererCommon::lightColour.push_back(glm::vec3{ 52.f, 52.f, 52.f });
	}

	void IntroScene::InitTextures()
	{
		// Initialize Textures
		Textures["Grime"] = std::shared_ptr<Texture>(Textures["Grime"]->create("assets/textures/Grime.png"));

		// Initialize Materials
		std::shared_ptr<Material> groundPlaneMat, texturedCube1Mat, skyBoxMat;

		groundPlaneMat.reset(new Material(Shaders["PBR"], RendererCommon::defaultTexture, {1.f, 1.f, 1.f, 1.f}, true));
		Materials["CameraMaterial"] = groundPlaneMat;
		texturedCube1Mat.reset(new Material(Shaders["PBR"], Textures["Grime"], { 1.f, 1.f, 1.f, 1.f }, true));
		Materials["TexturedCube1"] = texturedCube1Mat;
		skyBoxMat.reset(new Material(Shaders["PBR"], RendererCommon::defaultTexture, { 0.35f, 0.5f, 0.9f, 1.0f }, true));
		Materials["SkyBox"] = skyBoxMat;
	}

	void IntroScene::InitRenderCommands()
	{

		clearCommand.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::clearColourAndDepthBuffer));
		setClearColourCommand.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::setClearColour, 0.05f, 0.05f, 0.05f, 1.0f));
		glEnableBlend.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::Enable, GL_BLEND));
		glDisableBlend.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::Disable, GL_BLEND));
		glEnableDepthTest.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::Enable, GL_DEPTH_TEST));
		glDisableDepthTest.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::Disable, GL_DEPTH_TEST));
		setBlendFunc.reset(RenderCommandFactory::createCommand(RenderCommand::Commands::setBlendFunc));

	}

	void IntroScene::Render2D()
	{
		Renderer2D::begin(sWideUniforms2D);
		
		for (int i = 1; i < 9; i++)
		{
			const std::string& temp = ("Hotbar " + std::to_string(i));
			Renderer2D::submit(Quads[temp], *SubTextures["Hotbar"], {1.f, 0.3f, 0.4f, 0.7f});
		}

		Renderer2D::submit(Quads["Crosshair"], *SubTextures["Crosshair"], { 1.f, 0.3f, 0.4f, 0.7f });

		Renderer2D::submit(Quads["HealthBar"], { 1.f, 0.3f, 0.4f, 0.5f });

		Renderer2D::submit("Health Bar", { 10.f, 70.f }, { 1.f, 0.3f, 0.4f, 0.5f }, 25);
		
		Renderer2D::end();
	}

	void IntroScene::Render3D()
	{

		MaterialInfo Suzanne{ albedo, SuzanneValues.x, SuzanneValues.y, SuzanneValues.z };
		MaterialInfo Sphere{ albedo, SphereValues.x, SphereValues.y, SphereValues.z };
		MaterialInfo Floor{ albedo, FloorValues.x, FloorValues.y, FloorValues.z };

		Renderer3D::begin(sWideUniforms3D);

		Renderer3D::submit(Geometries["Plane"], Materials["SkyBox"], floorTransform, Floor);
		Renderer3D::submit(Geometries["Sphere"], Materials["CameraMaterial"], cameraTransform, Sphere);

		for (int i = 0; i < cubesDrawn; i++)
			Renderer3D::submit(Geometries["Suzanne"], Materials["TexturedCube1"], modelTransforms[i], Suzanne);

		Renderer3D::end();
	}

}