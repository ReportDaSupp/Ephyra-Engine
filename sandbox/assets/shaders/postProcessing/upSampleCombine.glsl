#region Compute
#version 440 core

layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) uniform readonly image2D sceneImage;
layout(binding = 4, rgba32f) uniform readonly image2D blurredImageV;
layout(binding = 5, rgba32f) uniform writeonly image2D finalImage;

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec4 sceneColor = imageLoad(sceneImage, pixelCoords);
    vec4 bloomColor = imageLoad(blurredImageV, pixelCoords);
    vec4 finalColor = sceneColor + bloomColor;
    
    imageStore(finalImage, pixelCoords, finalColor);
}
