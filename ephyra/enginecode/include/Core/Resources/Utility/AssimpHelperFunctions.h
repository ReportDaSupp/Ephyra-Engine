#pragma once

#include "Core/Resources/Management/ResourceManager.h"
#include "Core/Systems/Utility/Log.h"
#include "Core/Resources/Utility/GlobalAssimpData.h"

#include <glm/glm.hpp>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine {
	namespace Loader
	{
		glm::mat4 AssimpToGLMMatrix(const aiMatrix4x4& apMat) {
			return glm::mat4(
				apMat.a1, apMat.b1, apMat.c1, apMat.d1,
				apMat.a2, apMat.b2, apMat.c2, apMat.d2,
				apMat.a3, apMat.b3, apMat.c3, apMat.d3,
				apMat.a4, apMat.b4, apMat.c4, apMat.d4
			);
		}

		glm::vec3 AssimpToGLMVec(const aiVector3D& ai_vec) {
			return glm::vec3(ai_vec.x, ai_vec.y, ai_vec.z);
		}

		glm::quat AssimpToGLMQuat(const aiQuaternion& ai_quat) {
			return glm::quat(ai_quat.x, ai_quat.y, ai_quat.z, ai_quat.w);
		}

		const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string nodeName) {
			for (unsigned int i = 0; i < animation->mNumChannels; i++) {
				const aiNodeAnim* nodeAnim = animation->mChannels[i];
				if (std::string(nodeAnim->mNodeName.data) == nodeName) {
					return nodeAnim;
				}
			}
			return nullptr;
		}

		unsigned int findScalingIndex(float AnimationTime, const aiNodeAnim* pNodeAnim) {
			assert(pNodeAnim->mNumScalingKeys > 0);
			for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
				if (AnimationTime < float(pNodeAnim->mScalingKeys[i + 1].mTime)) {
					return i;
				}
			}
			assert(0);
			return 0;
		}

		unsigned int findRotationIndex(float AnimationTime, const aiNodeAnim* pNodeAnim) {
			assert(pNodeAnim->mNumRotationKeys > 0);
			for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
				if (AnimationTime < float(pNodeAnim->mRotationKeys[i + 1].mTime)) {
					return i;
				}
			}
			assert(0);
			return 0;
		}

		unsigned int findPositionIndex(float AnimationTime, const aiNodeAnim* pNodeAnim) {
			assert(pNodeAnim->mNumPositionKeys > 0);
			for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
				if (AnimationTime < float(pNodeAnim->mPositionKeys[i + 1].mTime)) {
					return i;
				}
			}
			assert(0);
			return 0;
		}

		glm::vec3 interpolateScaling(float time, const aiNodeAnim* nodeAnim) {
			if (nodeAnim->mNumScalingKeys == 1) {
				return AssimpToGLMVec(nodeAnim->mScalingKeys[0].mValue);
			}

			unsigned int index = findScalingIndex(time, nodeAnim);
			unsigned int nextIndex = index + 1;
			float deltaTime = nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime;
			float factor = (time - nodeAnim->mScalingKeys[index].mTime) / deltaTime;
			const aiVector3D& start = nodeAnim->mScalingKeys[index].mValue;
			const aiVector3D& end = nodeAnim->mScalingKeys[nextIndex].mValue;
			aiVector3D delta = end - start;
			aiVector3D interpolated = start + factor * delta;
			return AssimpToGLMVec(interpolated);
		}

		glm::quat interpolateRotation(float time, const aiNodeAnim* nodeAnim) {
			if (nodeAnim->mNumRotationKeys == 1) {
				return AssimpToGLMQuat(nodeAnim->mRotationKeys[0].mValue);
			}

			unsigned int index = findRotationIndex(time, nodeAnim);
			unsigned int nextIndex = index + 1;
			float deltaTime = nodeAnim->mRotationKeys[nextIndex].mTime - nodeAnim->mRotationKeys[index].mTime;
			float factor = (time - nodeAnim->mRotationKeys[index].mTime) / deltaTime;
			const aiQuaternion& start = nodeAnim->mRotationKeys[index].mValue;
			const aiQuaternion& end = nodeAnim->mRotationKeys[nextIndex].mValue;
			aiQuaternion interpolated;
			aiQuaternion::Interpolate(interpolated, start, end, factor);
			interpolated = interpolated.Normalize();
			return AssimpToGLMQuat(interpolated);
		}

		glm::vec3 interpolatePosition(float time, const aiNodeAnim* nodeAnim) {
			if (nodeAnim->mNumPositionKeys == 1) {
				return AssimpToGLMVec(nodeAnim->mPositionKeys[0].mValue);
			}

			unsigned int index = findPositionIndex(time, nodeAnim);
			unsigned int nextIndex = index + 1;
			float deltaTime = nodeAnim->mPositionKeys[nextIndex].mTime - nodeAnim->mPositionKeys[index].mTime;
			float factor = (time - nodeAnim->mPositionKeys[index].mTime) / deltaTime;
			const aiVector3D& start = nodeAnim->mPositionKeys[index].mValue;
			const aiVector3D& end = nodeAnim->mPositionKeys[nextIndex].mValue;
			aiVector3D delta = end - start;
			aiVector3D interpolated = start + factor * delta;
			return AssimpToGLMVec(interpolated);
		}
	}
}