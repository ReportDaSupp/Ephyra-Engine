#region Compute
#version 440 core
			
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32F) uniform readonly image2D sceneImage;
layout(binding = 1, rgba32F) uniform writeonly image2D finalImage;

uniform sampler2D depthTexture;

uniform float nearPlane = 0.01f; // Camera's near plane
uniform float farPlane = 100.f;  // Camera's far plane
uniform vec3 fogColor = vec3(0.7, 0.4, 0.4); // Fog color
uniform float fogDensity = 0.25; // Fog density

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));  
}

void main() {
    highp ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    highp vec2 texCoords = vec2(pixelCoords) / vec2(imageSize(sceneImage));

    float depth = texture2D(depthTexture, texCoords).r;

    float linearDepth = linearizeDepth(depth);
    float depthFactor = clamp((linearDepth - nearPlane) / (farPlane - nearPlane), 0.0, 1.0);

    float fogFactor = 1.0 - exp(-depthFactor * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    imageStore(finalImage, pixelCoords, vec4(mix(imageLoad(sceneImage, pixelCoords).xyz, fogColor, fogFactor),1.0));
	}
