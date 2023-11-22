#version 420 core

in vec2 Texcoord;

out vec4 outColor;

layout(binding = 0) uniform atomic_uint aints[2];

uniform sampler2D flareTex;
uniform sampler2D occludeTex;

void main()
{
	vec4 fc = min(vec4(1.0), texture(flareTex, Texcoord));
	bool bright = false;
	if ((fc.r * fc.g * fc.b * fc.a) > 0.9) {
		atomicCounterIncrement(aints[1]);
		bright = true;
	}
	if (floor(texture(occludeTex, Texcoord).a) == 0.0) {
		outColor = fc;
	} else {
		if (bright) atomicCounterIncrement(aints[0]);
		outColor = vec4(0.0);
	}
}