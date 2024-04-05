#region Compute
#version 440 core
			
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) uniform readonly image2D sceneImage;
layout(binding = 4, rgba32f) uniform readonly image2D blurredImage;
layout(binding = 5, rgba32f) uniform writeonly image2D finalImage;

uniform sampler2D depthTexture;

float focusDistance = 10.0;
uniform float focusRange = 1.0;
uniform float nearBlurStrength = 0.3;
uniform float farBlurStrength = 0.8;
uniform float nearPlane = 0.01f; // Camera's near plane
uniform float farPlane = 100.f;  // Camera's far plane

uniform vec2 screenSize;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));  
}

float calculateBlurAmount(float depth) {
    float depthDifference = abs(depth - focusDistance);
    if (depthDifference < focusRange) {
        return 0.0;
    }
    return mix(nearBlurStrength, farBlurStrength, step(focusDistance, depth)) * clamp((depthDifference - focusRange) / max(focusDistance, 1.0 - focusDistance), 0.0, 1.0);
}

void main() {
    vec2 centre = vec2(screenSize.x / 2, screenSize.y / 2);
    focusDistance = linearizeDepth(texture2D(depthTexture, centre).r);
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    highp vec2 texCoords = vec2(coords) / vec2(imageSize(sceneImage));
    float depth = texture2D(depthTexture, texCoords).r;
    float blurAmount = calculateBlurAmount(depth);

    vec4 sceneColor = imageLoad(sceneImage, coords);
    vec4 blurColor = imageLoad(blurredImage, coords);

    vec4 finalColor = mix(sceneColor, blurColor, blurAmount);
    
    imageStore(finalImage, coords, finalColor);
}