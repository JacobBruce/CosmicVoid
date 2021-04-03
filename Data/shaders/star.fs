#version 420 core

in vec4 Color;
in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D texSmplr;

void main()
{
	vec4 clrSample = texture(texSmplr, Texcoord);
	if (clrSample.a == 0.0) discard;
	float cs = max(0.08/length(Texcoord-0.5), 1.0);
	outColor = Color * clrSample * vec4(cs, cs, cs, 1.0);
}