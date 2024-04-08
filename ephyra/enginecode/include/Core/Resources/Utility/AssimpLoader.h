#pragma once

#include "Core/Resources/Management/ResourceManager.h"
#include "Core/Systems/Utility/Log.h"

#include <glm/glm.hpp>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace Engine {
	namespace Loader
	{
		struct TempMesh
		{
			
			std::vector<Renderer3DVertex>vertices;
			std::vector<uint32_t> indices;
			std::shared_ptr<Texture> diffuseTexture = nullptr;
			std::shared_ptr<Texture> specularTexture = nullptr;
			std::shared_ptr<Texture> ambientTexture = nullptr;
			std::shared_ptr<Texture> normalTexture = nullptr;
			glm::vec3 diffuseTint = { 1.f,1.f,1.f };

		};
		static std::shared_ptr<ResourceManager> s_resources = nullptr;
		static std::shared_ptr<Shader> s_shader = nullptr;

		static std::vector<TempMesh> s_preLoaded;
		static std::string s_filePath;
		static std::shared_ptr<Material> s_material = nullptr;
		static std::shared_ptr<Geometry> s_geo;
		static std::string s_ID;


		static void ASSIMPProcessMesh(aiMesh* mesh, const aiScene* scene)
		{

			TempMesh tmpMesh;

			tmpMesh.vertices.reserve(mesh->mNumVertices);

			std::multimap<uint32_t, std::pair<uint32_t, float>> vertexBoneWeights;

			// Find vertex properties
			bool hasPositions = mesh->HasPositions();
			bool hasNormals = mesh->HasNormals();
			bool hasTangents = mesh->HasTangentsAndBitangents();
			uint32_t numColourChannels = mesh->GetNumColorChannels();
			uint32_t numUVChannels = mesh->GetNumUVChannels();

			//Log::error("VERTICES");
			// Iterate through vertices
			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				// Get vertex data
				glm::vec3 position, normal, tangent, biTangent;
				std::vector<glm::vec4> colours(numColourChannels);
				std::vector<glm::vec2> texCoords(numUVChannels);

				if (hasPositions) position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				if (hasNormals) normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

				for (uint32_t j = 0; j < numColourChannels; j++)
				{
					colours[j].r = mesh->mColors[j][i].r;
					colours[j].g = mesh->mColors[j][i].g;
					colours[j].b = mesh->mColors[j][i].b;
					colours[j].a = mesh->mColors[j][i].a;
				}

				for (uint32_t j = 0; j < numUVChannels; j++)
				{
					texCoords[j] = glm::vec2(mesh->mTextureCoords[j][i].x, mesh->mTextureCoords[j][i].y);
				}

				tmpMesh.vertices.push_back(Renderer3DVertex(position, normal, texCoords[0]));


			}

			//Log::error("INDICES");
			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				// retrieve all indices of the face and store them in the indices vector
				for (uint32_t j = 0; j < face.mNumIndices; j++)
				{
					tmpMesh.indices.push_back(face.mIndices[j]);
				}
			}

			std::vector<aiTextureType> types =
			{
				aiTextureType_NONE,
				aiTextureType_DIFFUSE,
				aiTextureType_SPECULAR,
				aiTextureType_AMBIENT,
				aiTextureType_EMISSIVE,
				aiTextureType_HEIGHT,
				aiTextureType_NORMALS,
				aiTextureType_SHININESS,
				aiTextureType_OPACITY,
				aiTextureType_DISPLACEMENT,
				aiTextureType_LIGHTMAP,
				aiTextureType_REFLECTION,
				aiTextureType_BASE_COLOR,
				aiTextureType_NORMAL_CAMERA,
				aiTextureType_EMISSION_COLOR,
				aiTextureType_METALNESS,
				aiTextureType_DIFFUSE_ROUGHNESS,
				aiTextureType_AMBIENT_OCCLUSION,
				aiTextureType_UNKNOWN
			};

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			for (auto type : types)
			{
				for (uint32_t i = 0; i < material->GetTextureCount(type); i++)
				{
					aiString str;
					material->GetTexture(type, i, &str);

					if (type == aiTextureType_DIFFUSE)
					{
						std::string tempFilepath = s_filePath;
						tempFilepath.resize(s_filePath.rfind('/')+1);
						std::string fn(str.C_Str());
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						tmpMesh.diffuseTexture = s_resources->addAsset(s_ID + tag, SceneAsset::Type::Texture, tmp);

					}

					if (type == aiTextureType_SPECULAR)
					{
						std::string tempFilepath = s_filePath;
						tempFilepath.resize(s_filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						tmpMesh.specularTexture = s_resources->addAsset(s_ID + tag, SceneAsset::Type::Texture, tmp);

					}

					if (type == aiTextureType_AMBIENT)
					{
						std::string tempFilepath = s_filePath;
						tempFilepath.resize(s_filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						tmpMesh.ambientTexture = s_resources->addAsset(s_ID + tag, SceneAsset::Type::Texture, tmp);

					}

					if (type == aiTextureType_HEIGHT)
					{
						std::string tempFilepath = s_filePath;
						tempFilepath.resize(s_filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						tmpMesh.normalTexture = s_resources->addAsset(s_ID + tag, SceneAsset::Type::Texture, tmp);

					}


				}

			}


			aiString stringValue;
			int intValue;
			float floatValue;
			aiColor3D colorValue;

			//if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, stringValue)) Log::error("Material name: {0}", stringValue.C_Str());

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, colorValue))
			{
				if (!tmpMesh.diffuseTexture)
					tmpMesh.diffuseTint = { (float)colorValue.r, (float)colorValue.g, (float)colorValue.b };
				else
					tmpMesh.diffuseTint = glm::vec4(1.0f);

			}


			Engine::Geometry tmpGeo;
			//Sort out geometry and material
			Renderer3D::addGeometry(tmpMesh.vertices, tmpMesh.indices, tmpGeo);

			s_geo = std::make_shared<Geometry>(tmpGeo);

			if (tmpMesh.diffuseTexture)
			{
				if (!s_resources->getAsset<Material>(s_ID + "Material"))
				{
					std::vector<std::shared_ptr<Texture>> textures;
					textures.resize(5);
					
					for (int i = 0; i < 5; i++)
						textures[i] = RendererCommon::defaultTexture;
					
					textures[0] = tmpMesh.diffuseTexture;

					s_material.reset(new Material(s_shader, textures));

					//Add material to resource manager
					s_resources->addAsset(s_ID + "Material", Engine::SceneAsset::Type::Material, s_material);
				}
				else
				{
					auto& mat =  s_resources->getAsset<Material>(s_ID + "Material");
					mat->setTexture(tmpMesh.diffuseTexture, 0);
				}

			}
			if (tmpMesh.specularTexture)
			{
				if (!s_resources->getAsset<Material>(s_ID + "Material"))
				{
					std::vector<std::shared_ptr<Texture>> textures;
					textures.resize(5);
					
					for (int i = 0; i < 5; i++)
						textures[i] = RendererCommon::defaultTexture;
					
					textures[1] = tmpMesh.specularTexture;

					s_material.reset(new Material(s_shader, textures));

					//Add material to resource manager
					s_resources->addAsset(s_ID + "Material", Engine::SceneAsset::Type::Material, s_material);
				}
				else
				{
					auto& mat = s_resources->getAsset<Material>(s_ID + "Material");
					mat->setTexture(tmpMesh.specularTexture, 1);
				}

			}
			if (tmpMesh.ambientTexture)
			{
				if (!s_resources->getAsset<Material>(s_ID + "Material"))
				{
					std::vector<std::shared_ptr<Texture>> textures;
					textures.resize(5);
					
					for (int i = 0; i < 5; i++)
						textures[i] = RendererCommon::defaultTexture;
					s_material.reset(new Material(s_shader, textures));
					
					textures[3] = tmpMesh.ambientTexture;

					//Add material to resource manager
					s_resources->addAsset(s_ID + "Material", Engine::SceneAsset::Type::Material, s_material);
				}
				else
				{
					auto& mat = s_resources->getAsset<Material>(s_ID + "Material");
					mat->setTexture(tmpMesh.ambientTexture, 3);
				}

			}
			if (tmpMesh.normalTexture)
			{
				if (!s_resources->getAsset<Material>(s_ID + "Material"))
				{
					std::vector<std::shared_ptr<Texture>> textures;
					textures.resize(5);

					for (int i = 0; i < 5; i++)
						textures[i] = RendererCommon::defaultTexture;
					s_material.reset(new Material(s_shader, textures));

					textures[4] = tmpMesh.normalTexture;

					//Add material to resource manager
					s_resources->addAsset(s_ID + "Material", Engine::SceneAsset::Type::Material, s_material);
				}
				else
				{
					auto& mat = s_resources->getAsset<Material>(s_ID + "Material");
					mat->setTexture(tmpMesh.normalTexture, 4);
				}

			}
			if (!tmpMesh.diffuseTexture)
			{

				//s_material.reset(new Material(s_shader, RendererCommon::defaultTexture, glm::vec4(tmpMesh.diffuseTint, 1.0f)));

				//Add material to resource manager
				s_resources->addAsset(s_ID + "Material", Engine::SceneAsset::Type::Material, s_material);

			}

			//Add geometry to resource manager
			s_resources->addAsset(s_ID + "Geometry", Engine::SceneAsset::Type::Geometry, s_geo);

		}

		static void ASSIMPProcessNode(aiNode* node, const aiScene* scene)
		{
			std::string parentName = "Null";
			if (node->mParent != nullptr) parentName = node->mParent->mName.C_Str();
			//if (node->mNumMeshes > 0) Log::error("MESHED NODE: {0} PARENT: {1}", node->mName.C_Str(), parentName);
			//if (node->mNumMeshes == 0) Log::error("UNMESHED NODE: {0} PARENT: {1}", node->mName.C_Str(), parentName);

			aiMatrix4x4* transform = &node->mTransformation;

			//Log::error("TRANSFORM");
			//Log::error("{0} {1} {2} {3}", transform->a1, transform->a2, transform->a3, transform->a4);
			//Log::error("{0} {1} {2} {3}", transform->b1, transform->b2, transform->b3, transform->b4);
			//Log::error("{0} {1} {2} {3}", transform->c1, transform->c2, transform->c3, transform->c4);
			//Log::error("{0} {1} {2} {3}", transform->d1, transform->d2, transform->d3, transform->d4);

			// process all the node's meshes
			for (uint32_t i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				ASSIMPProcessMesh(mesh, scene);
			}

			//  Process child nodes
			for (uint32_t i = 0; i < node->mNumChildren; i++)
			{
				ASSIMPProcessNode(node->mChildren[i], scene);
			}
		}

		static void ASSIMPLoad(const std::string& filepath, std::string id, std::shared_ptr<Shader> shader = nullptr)
		{
			s_resources = Engine::ResourceManager::getInstance();
			s_ID = id;
			s_filePath = filepath;


			if (shader != nullptr)
			{
				s_shader = shader;
			}
			else
			{
				s_shader = s_resources->getAsset<Engine::Shader>("PBR");
			}

			Assimp::Importer importer;
			const aiScene* scene = importer.Assimp::Importer::ReadFile(filepath, aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_Triangulate);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				Log::error("Cannot load: {0}, ASSIMP Error {1}", filepath, importer.GetErrorString());
				return;
			}
			ASSIMPProcessNode(scene->mRootNode, scene);

		}
	}
}