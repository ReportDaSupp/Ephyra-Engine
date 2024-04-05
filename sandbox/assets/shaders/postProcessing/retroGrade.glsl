#region Compute
#version 440 core
			
layout (local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba32f) uniform readonly image2D sceneImage;
layout(binding = 1, rgba32f) uniform writeonly image2D finalImage;

uniform vec2 screenSize;
uniform float time = 1; // Use time to animate the grain
uniform float grainAmount = 0.025; // Intensity of the film grain
uniform float lineFrequency = 20; // Controls the frequency of the lines
uniform float lineIntensity = 0.08; // Intensity of the lines

// Random function to generate grain
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    ivec2 coords = ivec2(gl_GlobalInvocationID.xy);
    
    // Calculate a random value for the grain
    float grain = rand(vec2(coords.x, coords.y + time / 4)) * 2.0 - 1.0; // Grain in range [-1, 1]
    
    // Calculate line overlay based on y coordinate
    float lineValue = sin(float(coords.y + time /6) / screenSize.y * lineFrequency * 3.14159) * lineIntensity;
    
    // Retrieve the current color of the scene
    vec4 currentColor = imageLoad(sceneImage, coords);
    
    // Calculate the film grain intensity
    vec3 grainColor = currentColor.rgb + grain * grainAmount;
    
    // Apply the retro lines
    vec3 finalColor = mix(grainColor, vec3(lineValue), lineIntensity);
    
    // Write the final color to the image
    imageStore(finalImage, coords, vec4(finalColor, currentColor.a));
}
