#pragma once

#include <glm/glm.hpp>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

struct BoneInfo {
    glm::mat4 boneOffset;
    glm::mat4 finalTransformation;
};

inline static std::map<std::string, const aiScene*> sceneMapping;
inline static std::map<std::string, std::vector<BoneInfo>> boneInfoList;
inline static Assimp::Importer importer[100];

static class BoneMatrixManager
{
public:
    static glm::mat4* getBoneMatrices()
    {
        static glm::mat4 boneMatrices[100];
        return boneMatrices;
    }
} boneManager;