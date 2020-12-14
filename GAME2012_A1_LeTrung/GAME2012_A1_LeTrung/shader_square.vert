#version 430 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 model_matrix;

out vec3 color;

void main()
{
	gl_Position = model_matrix * vec4(vertex_position, 1.0);
	color = vertex_color; 
}
