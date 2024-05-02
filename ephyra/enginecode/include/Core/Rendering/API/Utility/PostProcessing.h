#pragma once

#include <cstdint>

namespace Engine {

    class PostProcessing {
    public:
        virtual ~PostProcessing() = default;

        virtual uint32_t ApplyBloomEffect(uint32_t sceneTexture) = 0;
        virtual uint32_t ApplyDOFEffect(uint32_t depthTexture, uint32_t sceneTexture) = 0;
        virtual uint32_t ApplyToneMappingEffect(uint32_t sceneTexture) = 0;
        virtual uint32_t ApplySoftShadowsEffect(uint32_t sceneTexture) = 0;
        virtual uint32_t ApplyVignetteEffect(uint32_t sceneTexture) = 0;
        virtual uint32_t ApplyVolumetricEffect(uint32_t depthTexture, uint32_t sceneTexture) = 0;
        virtual void updateColorFBO(uint32_t processedTexture, uint32_t outputTexture) = 0;

        static PostProcessing* create();

    };

}