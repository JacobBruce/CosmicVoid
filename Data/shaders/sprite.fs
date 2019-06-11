#version 420 core

in vec4 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texSmplr;

void main()
{
	outColor = Color * texture(texSmplr, Texcoord);
}