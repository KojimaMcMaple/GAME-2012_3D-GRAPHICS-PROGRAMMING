#version 430 core

in vec3 colour;
in vec2 texCoord;
out vec4 frag_colour;

uniform sampler2D texture0;

void main()
{
	frag_colour = texture(texture0, texCoord) * vec4(colour, 1.0f);
}