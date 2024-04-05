#region Compute
#version 440 core

layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) uniform readonly image2D sceneImage;
layout(binding = 1, rgba32f) uniform writeonly image2D finalImage;

// ACES Tone Mapping function
vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    vec3 hdrColor = imageLoad(sceneImage, coords).xyz;
    vec3 ldrColor = ACESFilm(hdrColor);

    imageStore(finalImage, coords, vec4(ldrColor, 1.0));
}