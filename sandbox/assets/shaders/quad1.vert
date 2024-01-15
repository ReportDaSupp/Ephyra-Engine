#version 440 core
			
layout(location = 0) in vec2 a_vertexPosition;
layout(location = 1) in vec2 a_texCoord;

out vec2 texCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	texCoord = a_texCoord;
	gl_Position =  u_projection * u_view * u_model * vec4(a_vertexPosition,1.0,1.0);
}

