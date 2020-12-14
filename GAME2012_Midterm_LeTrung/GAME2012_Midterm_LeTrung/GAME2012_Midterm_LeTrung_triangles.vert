#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;

out vec3 colour;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vertex_position, 1.0f);
	colour = vertex_colour;
}