#include "Ephyra_pch.h"
#include "Platform/OpenGl/OpenGLPostProcessing.h"
#include <Core/Initialization/GlobalProperties.h>
#include "Core/Rendering/API/Global/RendererCommon.h"

Engine::OpenGLPostProcessing::OpenGLPostProcessing()
{
    const char* VolumetricShaderSource = "assets/shaders/postProcessing/volumetric.glsl";
    
    const char* ColorExtractShaderSource = "assets/shaders/postProcessing/brightColorExtract.glsl";
    const char* DownSampleShaderSource = "assets/shaders/postProcessing/downSample.glsl";
    const char* GaussianBlurHShaderSource = "assets/shaders/postProcessing/gaussianBlurH.glsl";
    const char* GaussianBlurVShaderSource = "assets/shaders/postProcessing/gaussianBlurV.glsl";
    const char* UpSampleCombineShaderSource = "assets/shaders/postProcessing/upSampleCombine.glsl";
    const char* ToneMappingShaderSource = "assets/shaders/postProcessing/toneMapping.glsl";
    const char* VignetteShaderSource = "assets/shaders/postProcessing/vignette.glsl";
    const char* DOFShaderSource = "assets/shaders/postProcessing/depthOfField.glsl";
    const char* RetroGradeShaderSource = "assets/shaders/postProcessing/retroGrade.glsl";
    const char* CleanUpShaderSource = "assets/shaders/postProcessing/cleanUp.glsl";

    LoadVolumetricShader(VolumetricShaderSource);
    LoadBloomShader(ColorExtractShaderSource, DownSampleShaderSource, GaussianBlurHShaderSource, GaussianBlurVShaderSource, UpSampleCombineShaderSource);
    LoadToneMappingShader(ToneMappingShaderSource);
    LoadVignetteShader(VignetteShaderSource);
    LoadDOFShader(DOFShaderSource);
    LoadRetroGradeShader(RetroGradeShaderSource);
    LoadCleanUpShader(CleanUpShaderSource);
    
    BrightColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    DownSampleTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    BlurredTextureH.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    BlurredTextureV.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    BloomColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    DOFColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    ToneMappingColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    SoftShadowColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    VignetteColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    VolumetricColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
    RetroGradeColorTexture.reset(Texture::create(SCR_WIDTH, SCR_HEIGHT, 4, nullptr));
}

Engine::OpenGLPostProcessing::~OpenGLPostProcessing()
{

}

uint32_t Engine::OpenGLPostProcessing::ApplyBloomEffect(uint32_t sceneTexture)
{
    glUseProgram(ColorExtractShaderProgram->getID());

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, BrightColorTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


    glUseProgram(DownSampleShaderProgram->getID());

    glBindImageTexture(1, BrightColorTexture->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(2, DownSampleTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


    glUseProgram(GaussianBlurHShaderProgram->getID());

    glBindImageTexture(2, DownSampleTexture->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(3, BlurredTextureH->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


    glUseProgram(GaussianBlurVShaderProgram->getID());

    glBindImageTexture(3, BlurredTextureH->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(4, BlurredTextureV->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


    glUseProgram(UpSampleCombineShaderProgram->getID());

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(4, BlurredTextureV->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(5, BloomColorTexture->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    return BloomColorTexture->getID();
}

uint32_t Engine::OpenGLPostProcessing::ApplyDOFEffect(uint32_t depthTexture, uint32_t sceneTexture)
{
    glUseProgram(GaussianBlurHShaderProgram->getID());

    glBindImageTexture(2, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(3, BlurredTextureH->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


    glUseProgram(GaussianBlurVShaderProgram->getID());

    glBindImageTexture(3, BlurredTextureH->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(4, BlurredTextureV->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    

    glUseProgram(DOFShaderProgram->getID());

    glActiveTexture(GL_TEXTURE0 + 32);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(4, BlurredTextureV->getID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(5, DOFColorTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    DOFShaderProgram->uploadFloat2("screenSize", glm::vec2((float)SCR_WIDTH, (float)SCR_HEIGHT));
    DOFShaderProgram->uploadInt("depthTexture", 32);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    return DOFColorTexture->getID();
}

uint32_t Engine::OpenGLPostProcessing::ApplyToneMappingEffect(uint32_t sceneTexture)
{
    glUseProgram(ToneMappingShaderProgram->getID());

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, ToneMappingColorTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    return ToneMappingColorTexture->getID();
}

uint32_t Engine::OpenGLPostProcessing::ApplyVignetteEffect(uint32_t sceneTexture)
{
    glUseProgram(VignetteShaderProgram->getID());

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, VignetteColorTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    VignetteShaderProgram->uploadFloat2("screenSize", glm::vec2((float)SCR_WIDTH, (float)SCR_HEIGHT));

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    return VignetteColorTexture->getID();
}

uint32_t Engine::OpenGLPostProcessing::ApplyVolumetricEffect(uint32_t depthTexture, uint32_t sceneTexture)
{
    glUseProgram(VolumetricShaderProgram->getID());

    glActiveTexture(GL_TEXTURE0 + 32);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, VolumetricColorTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    VolumetricShaderProgram->uploadInt("depthTexture", 32);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    glActiveTexture(GL_TEXTURE0);

    return VolumetricColorTexture->getID();
}

uint32_t Engine::OpenGLPostProcessing::ApplyRetroGradeEffect(uint32_t sceneTexture, float frameCount)
{
    glUseProgram(RetroGradeShaderProgram->getID());

    glBindImageTexture(0, sceneTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, RetroGradeColorTexture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    RetroGradeShaderProgram->uploadFloat2("screenSize", glm::vec2((float)SCR_WIDTH, (float)SCR_HEIGHT));
    RetroGradeShaderProgram->uploadFloat("time", frameCount);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    return RetroGradeColorTexture->getID();
}

void Engine::OpenGLPostProcessing::updateColorFBO(uint32_t processedTexture, uint32_t outputTexture)
{
    glUseProgram(CleanUpShaderProgram->getID());

    glBindImageTexture(0, processedTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    glDispatchCompute(ceil((float)SCR_WIDTH / 16), ceil((float)SCR_HEIGHT / 16), 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Engine::OpenGLPostProcessing::LoadVolumetricShader(const char* shaderSource)
{
    VolumetricShaderProgram.reset(Engine::Shader::create(shaderSource));
}

void Engine::OpenGLPostProcessing::LoadBloomShader(const char* shaderSource1, const char* shaderSource2, const char* shaderSource3, const char* shaderSource4, const char* shaderSource5)
{
    ColorExtractShaderProgram.reset(Engine::Shader::create(shaderSource1));
    DownSampleShaderProgram.reset(Engine::Shader::create(shaderSource2));
    GaussianBlurHShaderProgram.reset(Engine::Shader::create(shaderSource3));
    GaussianBlurVShaderProgram.reset(Engine::Shader::create(shaderSource4));
    UpSampleCombineShaderProgram.reset(Engine::Shader::create(shaderSource5));
}

void Engine::OpenGLPostProcessing::LoadToneMappingShader(const char* shaderSource)
{
    ToneMappingShaderProgram.reset(Engine::Shader::create(shaderSource));
}

void Engine::OpenGLPostProcessing::LoadVignetteShader(const char* shaderSource)
{
    VignetteShaderProgram.reset(Engine::Shader::create(shaderSource));
}

void Engine::OpenGLPostProcessing::LoadDOFShader(const char* shaderSource)
{
    DOFShaderProgram.reset(Engine::Shader::create(shaderSource));
}

void Engine::OpenGLPostProcessing::LoadRetroGradeShader(const char* shaderSource)
{
    RetroGradeShaderProgram.reset(Engine::Shader::create(shaderSource));
}

void Engine::OpenGLPostProcessing::LoadCleanUpShader(const char* shaderSource)
{
    CleanUpShaderProgram.reset(Engine::Shader::create(shaderSource));
}
