#version 440 core
			
layout(location = 0) out vec4 colour;

in vec2 texCoord;

uniform vec4 u_tint;
uniform sampler2D u_texData;

void main()
{
	colour = texture(u_texData, texCoord) * u_tint;
}

