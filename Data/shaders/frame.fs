#version 420 core

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texSmplr;

void main()
{
	vec4 clrSample = texture(texSmplr, Texcoord);
	if (clrSample.w == 0.0) discard;
	outColor = clrSample;
}