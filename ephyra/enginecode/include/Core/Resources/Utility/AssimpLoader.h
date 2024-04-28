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
		struct BoneInfo {
			glm::mat4 boneOffset;
			glm::mat4 finalTransformation;
		};

		struct VertexBoneData {
			unsigned int IDs[4] = { 0 };
			float weights[4] = { 0.0f };

			void addBoneData(unsigned int boneID, float weight) {
				for (unsigned int i = 0; i < 4; ++i) {
					if (weights[i] == 0.0) {
						IDs[i] = boneID;
						weights[i] = weight;
						return;
					}
				}
			}
		};

		struct TempMesh
		{
			std::vector<Renderer3DVertex>vertices;
			std::vector<uint32_t> indices;
			std::vector<VertexBoneData> boneData;
			std::map<std::string, unsigned int> boneMapping;
			std::vector<BoneInfo> bones;
			unsigned int numBones = 0;
			std::shared_ptr<Texture> diffuseTexture = nullptr;
			std::shared_ptr<Texture> specularTexture = nullptr;
			std::shared_ptr<Texture> ambientTexture = nullptr;
			std::shared_ptr<Texture> normalTexture = nullptr;
			glm::vec3 diffuseTint = { 1.f,1.f,1.f };

		};

		struct Bone {
			std::string name;
			int id;
			glm::mat4 offsetMatrix;
			glm::mat4 finalTransformation;

			Bone() : id(-1), offsetMatrix(glm::mat4(1.0f)), finalTransformation(glm::mat4(1.0f)) {}
		};

		struct Keyframe {
			float timeStamp;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			Keyframe() : timeStamp(0.0f), position(glm::vec3(0.0f)), rotation(glm::quat()), scale(glm::vec3(1.0f)) {}
		};

		struct Animation {
			float duration;
			float ticksPerSecond;
			std::map<std::string, std::vector<Keyframe>> boneKeyframes;

			Animation() : duration(0.0f), ticksPerSecond(0.0f) {}
		};

		struct Skeleton {
			std::vector<Bone> bones;
			std::map<std::string, unsigned int> boneMapping;
			std::vector<Animation> animations;

			Skeleton() {}
		};

		static std::shared_ptr<ResourceManager> gResources = nullptr;
		static std::shared_ptr<Shader> s_shader = nullptr;
		static int meshCount = 0;

		glm::mat4 AssimpToGLMMatrix(const aiMatrix4x4& apMat) {
			glm::mat4 glmMat;

			glmMat[0][0] = apMat.a1; glmMat[1][0] = apMat.a2;
			glmMat[2][0] = apMat.a3; glmMat[3][0] = apMat.a4;
			glmMat[0][1] = apMat.b1; glmMat[1][1] = apMat.b2;
			glmMat[2][1] = apMat.b3; glmMat[3][1] = apMat.b4;
			glmMat[0][2] = apMat.c1; glmMat[1][2] = apMat.c2;
			glmMat[2][2] = apMat.c3; glmMat[3][2] = apMat.c4;
			glmMat[0][3] = apMat.d1; glmMat[1][3] = apMat.d2;
			glmMat[2][3] = apMat.d3; glmMat[3][3] = apMat.d4;

			return glmMat;
		}

		glm::vec3 AssimpToGLMVec(const aiVector3D& ai_vec) {
			return glm::vec3(ai_vec.x, ai_vec.y, ai_vec.z);
		}

		static void processAnimations(const aiScene* scene, Skeleton& skeleton) {
			for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* ai_anim = scene->mAnimations[i];
				Animation anim;
				anim.duration = ai_anim->mDuration;
				anim.ticksPerSecond = ai_anim->mTicksPerSecond ? ai_anim->mTicksPerSecond : 25.0f;

				for (unsigned int j = 0; j < ai_anim->mNumChannels; j++) {
					aiNodeAnim* channel = ai_anim->mChannels[j];
					std::vector<Keyframe> keyframes;

					// Process all keyframes for this channel
					for (unsigned int k = 0; k < channel->mNumPositionKeys; k++) {
						Keyframe keyframe;
						keyframe.timeStamp = (float)channel->mPositionKeys[k].mTime;
						keyframe.position = AssimpToGLMVec(channel->mPositionKeys[k].mValue);

						if (k < channel->mNumRotationKeys) {
							keyframe.rotation = glm::quat(
								channel->mRotationKeys[k].mValue.w,
								channel->mRotationKeys[k].mValue.x,
								channel->mRotationKeys[k].mValue.y,
								channel->mRotationKeys[k].mValue.z
							);
						}

						if (k < channel->mNumScalingKeys) {
							keyframe.scale = AssimpToGLMVec(channel->mScalingKeys[k].mValue);
						}

						keyframes.push_back(keyframe);
					}

					anim.boneKeyframes[channel->mNodeName.C_Str()] = keyframes;
				}

				skeleton.animations.push_back(anim);
			}
		}

		static void ASSIMPProcessMesh(aiMesh* mesh, const aiScene* scene, std::string ID, std::string filePath)
		{
			gResources->MappedIDs[filePath].push_back(ID);
			
			TempMesh tmpMesh;

			tmpMesh.vertices.reserve(mesh->mNumVertices);

			bool hasPositions = mesh->HasPositions();
			bool hasNormals = mesh->HasNormals();
			bool hasTangents = mesh->HasTangentsAndBitangents();
			bool hasBones = mesh->HasBones();
			uint32_t numColourChannels = mesh->GetNumColorChannels();
			uint32_t numUVChannels = mesh->GetNumUVChannels();

			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				glm::vec3 position, normal, tangent, biTangent;
				glm::vec4 boneIndices, boneWeights;
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

				tmpMesh.vertices.push_back(Renderer3DVertex(position, normal, texCoords[0], boneIndices, boneWeights));


			}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
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
						std::string tempFilepath = filePath;
						tempFilepath.resize(filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						tmpMesh.diffuseTexture = gResources->addAsset(ID + tag, SceneAsset::Type::Texture, tmp);
					}

					if (type == aiTextureType_SPECULAR)
					{
						std::string tempFilepath = filePath;
						tempFilepath.resize(filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						tmpMesh.specularTexture = gResources->addAsset(ID + tag, SceneAsset::Type::Texture, tmp);
					}

					if (type == aiTextureType_AMBIENT)
					{
						std::string tempFilepath = filePath;
						tempFilepath.resize(filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						tmpMesh.ambientTexture = gResources->addAsset(ID + tag, SceneAsset::Type::Texture, tmp);

					}

					if (type == aiTextureType_HEIGHT)
					{
						std::string tempFilepath = filePath;
						tempFilepath.resize(filePath.rfind('/') + 1);
						std::string fn(str.C_Str());
						std::string tag;
						for (int i = fn.rfind('/') + 1; i < fn.size(); i++)
							tag += fn[i];
						tag.resize(tag.rfind('.'));
						std::shared_ptr<Texture> tmp;
						tmp = std::shared_ptr<Texture>(tmp->create((tempFilepath + fn).c_str()));
						tmpMesh.normalTexture = gResources->addAsset(ID + tag, SceneAsset::Type::Texture, tmp);
					}

				}

			}

			aiString stringValue;
			int intValue;
			float floatValue;
			aiColor3D colorValue;

			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, colorValue))
			{
				if (!tmpMesh.diffuseTexture)
					tmpMesh.diffuseTint = { (float)colorValue.r, (float)colorValue.g, (float)colorValue.b };
				else
					tmpMesh.diffuseTint = glm::vec4(1.0f);

			}

			Engine::Geometry tmpGeo;

			Renderer3D::addGeometry(tmpMesh.vertices, tmpMesh.indices, tmpGeo);
			gResources->addAsset(ID + "Geometry", Engine::SceneAsset::Type::Geometry, std::make_shared<Geometry>(tmpGeo));

			std::vector<std::shared_ptr<Texture>> textures;
			textures.resize(5);

			for (int i = 0; i < 5; i++)
				textures[i] = RendererCommon::defaultTexture;


			if (tmpMesh.diffuseTexture)
			{
				textures[0] = tmpMesh.diffuseTexture;
			}
			if (tmpMesh.specularTexture)
			{
				textures[1] = tmpMesh.specularTexture;
			}
			if (tmpMesh.ambientTexture)
			{
				textures[3] = tmpMesh.ambientTexture;
			}
			if (tmpMesh.normalTexture)
			{
				textures[4] = tmpMesh.normalTexture;
			}
			gResources->addAsset(ID + "Material", Engine::SceneAsset::Type::Material, std::make_shared<Material>(s_shader, textures, false));

		}

		static void ASSIMPProcessNode(aiNode* node, const aiScene* scene, std::string ID, std::string filepath)
		{
			std::string parentName = "Null";
			if (node->mParent != nullptr) parentName = node->mParent->mName.C_Str();

			aiMatrix4x4* transform = &node->mTransformation;

			// process all the node's meshes
			for (uint32_t i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				ASSIMPProcessMesh(mesh, scene, mesh->mName.C_Str(), filepath);
			}

			//  Process child nodes
			for (uint32_t i = 0; i < node->mNumChildren; i++)
			{
				ASSIMPProcessNode(node->mChildren[i], scene, ID, filepath);
			}
		}

		static void ASSIMPLoad(const std::string& filepath, std::string id, std::shared_ptr<Shader> shader = nullptr)
		{
			gResources = Engine::ResourceManager::getInstance();
			meshCount = 0;

			if (!(gResources->MappedIDs.find(filepath) == gResources->MappedIDs.end()))
				for (int i = 0; i < gResources->MappedIDs[filepath].size(); i++)
				{
					auto& mat = gResources->getAsset<Material>(gResources->MappedIDs[filepath][i] + "Material");
					if (!mat->isFlagSet(1))
						mat->setFlag(1);
					return;
				}

			if (shader != nullptr)
			{
				s_shader = shader;
			}
			else
			{
				s_shader = gResources->getAsset<Engine::Shader>("PBR");
			}

			Assimp::Importer importer;
			const aiScene* scene = importer.Assimp::Importer::ReadFile(filepath, aiProcess_FlipUVs | aiProcess_Triangulate);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				Log::error("Cannot load: {0}, ASSIMP Error {1}", filepath, importer.GetErrorString());
				return;
			}

			ASSIMPProcessNode(scene->mRootNode, scene, id, filepath);
			

		}
	}
}