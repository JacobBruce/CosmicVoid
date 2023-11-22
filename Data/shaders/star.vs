#version 420 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 color;
layout(location=3) in vec2 offset;
layout(location=4) in float scale;
layout(location=5) in float depth;

out vec4 Color;
out vec2 Texcoord;
out float Strength;

uniform mat4 projection;

void main()
{
	Color = color;
	Strength = pow(1.0-depth, 13.0) * scale * 0.2;
	Texcoord = texcoord;
	vec4 result = projection * vec4((scale*position)+offset, 0.0, 1.0);
	result.z = depth;
	gl_Position = result;
}