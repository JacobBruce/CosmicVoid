#version 420 core

in vec2 Texcoord;

out vec4 outColor;

uniform vec2 direction;
uniform float opacity;

uniform sampler2D texSmplr;
uniform sampler2D occludeTex;

const float OFFSETS[12] = float[12](
    -6, -5, -4, -3, -2, -1,
    1, 2, 3, 4, 5, 6
);

const float WEIGHTS[12] = float[12](
    0.018815730430644367,
    0.03447396964662017,
    0.05657737457255749,
    0.08317258170844943,
    0.10952340502389689,
    0.12918787500405665,
    0.12918787500405665,
    0.10952340502389689,
    0.08317258170844943,
    0.05657737457255749,
    0.03447396964662017,
    0.018815730430644367
);

void main()
{
	vec4 clrSample = texture(texSmplr, Texcoord);
	vec4 result = clrSample * 0.13649812722755;
	vec2 size = textureSize(texSmplr, 0);
	vec2 pixCoord;
	
	if (floor(texture(occludeTex, Texcoord).a) == 1.0) {
	
		for (int i=0; i<12; ++i)
		{
			pixCoord = Texcoord + (direction * OFFSETS[i] * 1.5 / size);
			result += texture(texSmplr, pixCoord) * WEIGHTS[i];
		}
		
		if (direction.y == 1.0)
			result = min(vec4(1.0), result*1.1);
		
	} else { // ensure transparency doesn't blur out of occluded areas
	
		for (int i=0; i<12; ++i)
		{
			pixCoord = Texcoord + (direction * OFFSETS[i] / size);
			if (floor(texture(occludeTex, pixCoord).a) == 0.0) {
				result += texture(texSmplr, pixCoord) * WEIGHTS[i];
			} else {
				result += clrSample * WEIGHTS[i];
			}
		}
		
	}
	
    outColor = result * opacity;
}