#pragma once

#include <Ephyra.h>

#include "Core/Rendering/Renderer/Renderer2D.h"
#include "Core/Rendering/Renderer/Renderer3D.h"
#include "Core/Resources/Management/ResourceManager.h"
#include "Core/Resources/Utility/AssimpLoader.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

namespace Engine 
{
	enum class TagType {
		Undefined = 0,
		Render2D,
		Render3D,
		Light
	};

	// Loaded
	struct TransformComponent
	{
		glm::mat4 Transform;
		glm::vec3 Translation;
		glm::quat Rotation;
		glm::vec3 Euler;
		glm::vec3 Scale;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform) : Transform(transform) {}
		TransformComponent(glm::vec3 T, glm::vec3 E, glm::vec3 S) { 
			Translation = T;
			glm::mat4 t = glm::translate(glm::mat4(1.0f), T);
			Euler = E;
			Rotation = glm::quat(E);
			glm::mat4 r = glm::mat4_cast(Rotation);
			Scale = S;
			glm::mat4 s = glm::scale(glm::mat4(1.0f), S);
			Transform = t * r * s; }

		operator glm::mat4& () { return Transform; }
		operator const glm::mat4& () const { return Transform; }
	};

	struct MeshRendererComponent
	{
		std::vector<std::shared_ptr<Engine::Material>> Material;
		std::vector<std::shared_ptr<Engine::Geometry>> Geometry;

		std::string LoaderPath;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;
		MeshRendererComponent(std::string filepath, std::string ID) 
		{ 
			std::shared_ptr<ResourceManager> resources;
			resources = ResourceManager::getInstance();
			Engine::Loader::ASSIMPLoad(filepath, ID); 
			for (int i = 0; i < resources->IDToMeshNames[resources->FPToIDs[filepath][0]].size(); i++)
			{
				std::string tempID = resources->IDToMeshNames[resources->FPToIDs[filepath][0]][i];
				Geometry.push_back(resources->getAsset<Engine::Geometry>(tempID + "Geometry"));
				Material.push_back(resources->getAsset<Engine::Material>(tempID + "Material"));
			}
			LoaderPath = filepath;
		}
	};

	struct SpriteRendererComponent
	{
		std::shared_ptr<Engine::Quad> Quad;
		glm::vec2 Position;
		std::shared_ptr<Engine::SubTexture> SubTexture;
		glm::vec4 Color;
		std::string Text;
		uint32_t Type;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const std::shared_ptr<Engine::Quad>& quad, const glm::vec2& pos, const std::shared_ptr<Engine::SubTexture>& subtexture) : Quad(quad), Position(pos), SubTexture(subtexture) { Type = 1; }	// Type 1 is Textured
		SpriteRendererComponent(const std::shared_ptr<Engine::Quad>& quad, const glm::vec2& pos, const glm::vec4& color) : Quad(quad), Position(pos), Color(color) { Type = 2; }											// Type 2 is Colored
		SpriteRendererComponent(const std::shared_ptr<Engine::Quad>& quad, const glm::vec2& pos, const glm::vec4& color, const std::string& text) : Quad(quad), Position(pos), Color(color), Text(text) { Type = 3; }		// Type 3 is Text Colored

	};

	struct EmmissiveComponent {
		glm::vec3 Color;
		glm::vec3 Position;

		EmmissiveComponent() = default;
		EmmissiveComponent(const EmmissiveComponent&) = default;
		EmmissiveComponent(glm::vec3 color, glm::vec3 relPosition) : Color(color), Position(relPosition) {}

	};

	// Loaded
	struct TagComponent
	{
		std::string Tag;
		TagType Type;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag, const TagType& type) : Tag(tag), Type(type) {}
		TagComponent(const std::string& tag, const int& type) : Tag(tag), Type(TagType(type)) {}

		operator std::string& () { return Tag; }
	};

	struct StateComponent
	{
		bool State;

		StateComponent() = default;
		StateComponent(const StateComponent&) = default;
		StateComponent(bool State) : State(State) {}

		operator bool& () { return State; }
	};
}