#pragma once

#include "Core/Resources/Utility/AssimpHelperFunctions.h"
#include <glm/gtx/integer.hpp>

namespace Engine {
	namespace Loader
	{

		struct VertexBoneData {
			glm::vec4 IDs;
			glm::vec4 Weights;

			void addBoneData(int BoneID, float Weight) {
				for (int i = 0; i < 4; i++) {
					if (Weights[i] == 0.0) {
						IDs[i] = BoneID;
						Weights[i] = Weight;
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
			std::shared_ptr<Texture> diffuseTexture = nullptr;
			std::shared_ptr<Texture> specularTexture = nullptr;
			std::shared_ptr<Texture> ambientTexture = nullptr;
			std::shared_ptr<Texture> normalTexture = nullptr;
			glm::vec3 diffuseTint = { 1.f,1.f,1.f };

		};

		static std::shared_ptr<ResourceManager> gResources = nullptr;
		static std::shared_ptr<Shader> s_shader = nullptr;
		
		std::map<std::string, unsigned int> boneMapping;
		unsigned int numBones = 0;

		static void updateBoneTransforms(float timeInSeconds, const aiNode* pNode, const glm::mat4& parentTransform, std::string ID) {
			std::string nodeName(pNode->mName.data);
			glm::mat4 nodeTransformation = AssimpToGLMMatrix(pNode->mTransformation);

			const aiAnimation* animation = sceneMapping[ID]->mAnimations[0];  // Assuming using the first animation
			const aiNodeAnim* nodeAnim = findNodeAnim(animation, nodeName);

			if (nodeAnim) {
				// Interpolate scaling, rotation, and translation
				glm::vec3 scaling = interpolateScaling(glm::mod(timeInSeconds, (float)(sceneMapping[ID]->mAnimations[0]->mDuration / sceneMapping[ID]->mAnimations[0]->mTicksPerSecond)), nodeAnim);
				glm::mat4 scalingM = glm::scale(glm::mat4(1.0f), scaling);

				glm::quat rotationQ = interpolateRotation(glm::mod(timeInSeconds, (float)(sceneMapping[ID]->mAnimations[0]->mDuration / sceneMapping[ID]->mAnimations[0]->mTicksPerSecond)), nodeAnim);
				glm::mat4 rotationM = glm::mat4(rotationQ);

				glm::vec3 translation = interpolatePosition(glm::mod(timeInSeconds, (float)(sceneMapping[ID]->mAnimations[0]->mDuration / sceneMapping[ID]->mAnimations[0]->mTicksPerSecond)), nodeAnim);
				glm::mat4 translationM = glm::translate(glm::mat4(1.0f), translation);

				// Combine the transformations
				nodeTransformation *= scalingM;
				nodeTransformation *= rotationM;
				nodeTransformation *= translationM;
			}

			glm::mat4 globalTransformation =  parentTransform * nodeTransformation;

			if (!(boneMapping.find(nodeName) == boneMapping.end())) {
				unsigned int boneIndex = boneMapping[nodeName];
				glm::mat4 bOMat = boneInfoList[sceneMapping[ID]->mMeshes[0]->mName.data][boneIndex].boneOffset;
				glm::mat4 tMat = AssimpToGLMMatrix(sceneMapping[ID]->mRootNode->mTransformation);
				glm::mat4 iTMat = glm::inverse(tMat);

				boneInfoList[sceneMapping[ID]->mMeshes[0]->mName.data][boneIndex].finalTransformation = glm::mat4(1.0f);
				//boneInfoList[sceneMapping[ID]->mMeshes[0]->mName.data][boneIndex].finalTransformation *= bOMat;
				//boneInfoList[sceneMapping[ID]->mMeshes[0]->mName.data][boneIndex].finalTransformation *= globalTransformation;
				//boneInfoList[sceneMapping[ID]->mMeshes[0]->mName.data][boneIndex].finalTransformation *= iTMat;
				
			}

			for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
				updateBoneTransforms(timeInSeconds, pNode->mChildren[i], globalTransformation, ID);
			}

		}

		static void ASSIMPProcessMesh(aiMesh* mesh, const aiScene* scene, std::string ID, std::string filePath)
		{
			gResources->IDToMeshNames[ID].push_back(mesh->mName.C_Str());
			
			TempMesh tmpMesh;
			tmpMesh.boneData.resize(mesh->mNumVertices);
			tmpMesh.vertices.reserve(mesh->mNumVertices);

			bool hasPositions = mesh->HasPositions();
			bool hasNormals = mesh->HasNormals();
			bool hasTangents = mesh->HasTangentsAndBitangents();
			bool hasBones = mesh->HasBones();
			uint32_t numColourChannels = mesh->GetNumColorChannels();
			uint32_t numUVChannels = mesh->GetNumUVChannels();

			for (unsigned int i = 0; i < mesh->mNumBones; i++) {
				unsigned int boneIndex = 0;
				std::string boneName = mesh->mBones[i]->mName.C_Str();

				if (boneMapping.find(boneName) == boneMapping.end()) {
					// Allocate an index for a new bone
					boneIndex = numBones;
					numBones++;
					BoneInfo bi;
					bi.boneOffset = AssimpToGLMMatrix(mesh->mBones[i]->mOffsetMatrix);
					boneInfoList[mesh->mName.C_Str()].push_back(bi);
					boneMapping[boneName] = boneIndex;
				}
				else {
					boneIndex = boneMapping[boneName];
				}

				for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
					unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
					float Weight = mesh->mBones[i]->mWeights[j].mWeight;
					tmpMesh.boneData[VertexID].addBoneData(boneIndex, Weight);
				}
				
			}

			for (unsigned int x = 0; x < tmpMesh.boneData.size(); x++) {
					float sum = 0.0f;
					float totalWeight = tmpMesh.boneData[x].Weights.x + tmpMesh.boneData[x].Weights.y + tmpMesh.boneData[x].Weights.z + tmpMesh.boneData[x].Weights.w;

					if (totalWeight > 0)
					{
						tmpMesh.boneData[x].Weights.x /= totalWeight;
						tmpMesh.boneData[x].Weights.y /= totalWeight;
						tmpMesh.boneData[x].Weights.z /= totalWeight;
						tmpMesh.boneData[x].Weights.w /= totalWeight;
					}
				}

			for (uint32_t i = 0; i < mesh->mNumVertices; i++)
			{
				glm::vec3 position, normal, tangent, biTangent;
				glm::vec4 boneIndices, boneWeights;
				std::vector<glm::vec4> colours(numColourChannels);
				std::vector<glm::vec2> texCoords(numUVChannels);

				if (hasPositions) position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				if (hasNormals) normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				if (hasBones)
				{
					boneIndices = tmpMesh.boneData[i].IDs;
					boneWeights = tmpMesh.boneData[i].Weights;
				}

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
						tmpMesh.diffuseTexture = gResources->addAsset(mesh->mName.C_Str() + tag, SceneAsset::Type::Texture, tmp);
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
						tmpMesh.specularTexture = gResources->addAsset(mesh->mName.C_Str() + tag, SceneAsset::Type::Texture, tmp);
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
						tmpMesh.ambientTexture = gResources->addAsset(mesh->mName.C_Str() + tag, SceneAsset::Type::Texture, tmp);

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
						tmpMesh.normalTexture = gResources->addAsset(mesh->mName.C_Str() + tag, SceneAsset::Type::Texture, tmp);
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
			std::string name = mesh->mName.C_Str();
			gResources->addAsset(name + ("Geometry"), Engine::SceneAsset::Type::Geometry, std::make_shared<Geometry>(tmpGeo));

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
			gResources->addAsset(name + "Material", Engine::SceneAsset::Type::Material, std::make_shared<Material>(s_shader, textures, false));

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
				ASSIMPProcessMesh(mesh, scene, ID, filepath);
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

			if (gResources->FPToIDs.find(filepath) != gResources->FPToIDs.end())
				for (int i = 0; i < gResources->IDToMeshNames[gResources->FPToIDs[filepath][0]].size(); i++)
				{
					auto& mat = gResources->getAsset<Material>(gResources->IDToMeshNames[gResources->FPToIDs[filepath][0]][i] + "Material");
					if (!mat->isFlagSet(mat->flag_batched))
						mat->setFlag(1);
					return;
				}
			else
				gResources->FPToIDs[filepath].push_back(id);

			if (shader != nullptr)
			{
				s_shader = shader;
			}
			else
			{
				s_shader = gResources->getAsset<Engine::Shader>("PBR");
			}
			
			if (sceneMapping.find(id) == sceneMapping.end()) {
				const aiScene* temp = importer[gResources->fileCount].ReadFile(filepath,
					aiProcess_Triangulate |
					aiProcess_FlipUVs |
					aiProcess_GenSmoothNormals |
					aiProcess_LimitBoneWeights |
					aiProcess_JoinIdenticalVertices);
				if (!temp || temp->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !temp->mRootNode)
				{
					Log::error("Cannot load: {0}, ASSIMP Error {1}", filepath, importer[gResources->fileCount].GetErrorString());
					return;
				}
				else {
					sceneMapping[id] = importer[gResources->fileCount].GetScene();
					gResources->fileCount++;
				}
			}

			ASSIMPProcessNode(sceneMapping[id]->mRootNode, sceneMapping[id], id, filepath);
			

		}
	}
}