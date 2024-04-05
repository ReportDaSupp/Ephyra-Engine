#region Compute
#version 440 core
			
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) uniform readonly image2D sceneImage;
layout(binding = 1, rgba32f) uniform writeonly image2D finalImage;

uniform float vignetteIntensity = 0.12;
uniform float vignetteRadius = 0.8;
uniform float fallOffExponent = 3.0;
uniform vec2 screenSize;

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 position = (vec2(coords) - 0.5 * screenSize) / (0.5 * screenSize);
    float distance = length(position); // Distance from the center

    float vignette = 1.0 - pow(clamp(distance / vignetteRadius, 0.0, 1.0), fallOffExponent);
    vignette = mix(1.0, vignette, vignetteIntensity);

    vec4 sceneColor = imageLoad(sceneImage, coords);
    vec4 finalColor = sceneColor * vignette;

    imageStore(finalImage, coords, finalColor);

}