#region Compute
#version 440 core
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) uniform readonly image2D sceneImage;
layout(binding = 1, rgba32f) uniform writeonly image2D brightAreasImage;

uniform float brightnessThreshold = 0.75;

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    vec4 color = imageLoad(sceneImage, coords);
    float luminance = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (luminance > brightnessThreshold) {
        imageStore(brightAreasImage, coords, color);
    } else {
        imageStore(brightAreasImage, coords, vec4(0.0, 0.0, 0.0, 1.0));
    }
}
