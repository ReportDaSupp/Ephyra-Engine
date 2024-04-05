#region Compute
#version 440 core

layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 1, rgba32f) uniform readonly image2D brightAreasImage;
layout(binding = 2, rgba32f) uniform writeonly image2D downsampledImage;

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    vec4 colorSum = vec4(0.0);
    for (int y = 0; y < 2; ++y) {
        for (int x = 0; x < 2; ++x) {
            colorSum += imageLoad(brightAreasImage, coords + ivec2(x, y));
        }
    }
    imageStore(downsampledImage, ivec2(gl_GlobalInvocationID.xy), colorSum / 4.0);
}
