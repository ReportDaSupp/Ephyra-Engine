#region Compute
#version 440 core

layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 2, rgba32f) uniform readonly image2D downsampledImage;
layout(binding = 3, rgba32f) uniform writeonly image2D blurredImageH;

uniform int blurRadius = 25;

vec4 gaussianBlur(ivec2 coords) {
    vec4 sum = vec4(0.0);
    float weightSum = 0.0;

    for (int i = -blurRadius; i <= blurRadius; ++i) {
        float weight = exp(-float(i * i) / (2.0 * blurRadius * blurRadius));
        weightSum += weight;

        ivec2 sampleCoords = coords + ivec2(i, 0);
        vec4 blur = imageLoad(downsampledImage, sampleCoords);
        sum += blur * weight;
    }

    return sum / weightSum;
}

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);

    vec4 horizBlurred = gaussianBlur(coords);

    imageStore(blurredImageH, coords, horizBlurred);
}