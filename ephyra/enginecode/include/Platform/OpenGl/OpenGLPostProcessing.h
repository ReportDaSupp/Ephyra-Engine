#pragma once

#include "Core/Rendering/API/Utility/PostProcessing.h"
#include <glad/glad.h>
#include "OpenGLShader.h"
#include "OpenGLTexture.h"

namespace Engine {

    class OpenGLPostProcessing : public PostProcessing {
    public:
        OpenGLPostProcessing();
        virtual ~OpenGLPostProcessing();

        virtual uint32_t ApplyBloomEffect(uint32_t sceneTexture) override;
        virtual uint32_t ApplyDOFEffect(uint32_t depthTexture, uint32_t sceneTexture) override;
        virtual uint32_t ApplyToneMappingEffect(uint32_t sceneTexture) override;
        virtual uint32_t ApplySoftShadowsEffect(uint32_t sceneTexture) override { return 0; };
        virtual uint32_t ApplyVignetteEffect(uint32_t sceneTexture) override;
        virtual uint32_t ApplyVolumetricEffect(uint32_t depthTexture, uint32_t sceneTexture) override;
        virtual void updateColorFBO(uint32_t processedTexture, uint32_t outputTexture) override;

    private:
        std::shared_ptr<Texture> BrightColorTexture;
        std::shared_ptr<Texture> DownSampleTexture;
        std::shared_ptr<Texture> BlurredTextureH;
        std::shared_ptr<Texture> BlurredTextureV;
        std::shared_ptr<Texture> BloomColorTexture;
        std::shared_ptr<Texture> DOFColorTexture;
        std::shared_ptr<Texture> ToneMappingColorTexture;
        std::shared_ptr<Texture> SoftShadowColorTexture;
        std::shared_ptr<Texture> VignetteColorTexture;
        std::shared_ptr<Texture> VolumetricColorTexture;

        std::shared_ptr<Shader> VolumetricShaderProgram; 

        std::shared_ptr<Shader> ColorExtractShaderProgram; std::shared_ptr<Shader> DownSampleShaderProgram; std::shared_ptr<Shader> GaussianBlurHShaderProgram; std::shared_ptr<Shader> GaussianBlurVShaderProgram; std::shared_ptr<Shader> UpSampleCombineShaderProgram;

        std::shared_ptr<Shader> ToneMappingShaderProgram;

        std::shared_ptr<Shader> VignetteShaderProgram;

        std::shared_ptr<Shader> DOFShaderProgram;

        std::shared_ptr<Shader> CleanUpShaderProgram;

        void LoadVolumetricShader(const char* shaderSource);
        void LoadBloomShader(const char* shaderSource1, const char* shaderSource2, const char* shaderSource3, const char* shaderSource4, const char* shaderSource5);
        void LoadToneMappingShader(const char* shaderSource);
        void LoadVignetteShader(const char* shaderSource);
        void LoadDOFShader(const char* shaderSource);
        void LoadCleanUpShader(const char* shaderSource);

    };

}