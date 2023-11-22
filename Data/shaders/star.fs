#version 420 core

in vec4 Color;
in vec2 Texcoord;
in float Strength;

out vec4 outColor;

uniform sampler2D texSmplr;

void main()
{
	vec4 clrSample = texture(texSmplr, Texcoord);
	float d = length(Texcoord-0.5);
	float cs = max(0.08/d, 1.0);
	vec3 bc = (Color.xyz + 1.0) * 0.5;
	vec4 bloom = vec4(bc, pow(max(0.0,0.5-d), 2.0) * Strength);
	if (clrSample.a == 0.0 && bloom.a == 0.0) discard;
	outColor = (bloom + (Color * clrSample * vec4(cs,cs,cs, 1.0))) * 0.5;
}