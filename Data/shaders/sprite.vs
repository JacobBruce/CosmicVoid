#version 420 core

layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 color;
layout(location=3) in vec2 offset;
layout(location=4) in float scale;
layout(location=5) in float rotation;
layout(location=6) in float depth;

out vec4 Color;
out vec2 Texcoord;

uniform mat4 projection;

vec2 rotate(vec2 v, float angle)
{
	float cosine = cos(angle);
	float sine = sin(angle);
	return vec2((v.x*cosine)-(v.y*sine), (v.x*sine)+(v.y*cosine));
}

void main()
{
	Color = color;
	Texcoord = texcoord;
	vec4 result = projection * vec4(rotate(scale*position,rotation)+offset, 0.0, 1.0);
	result.z = depth;
	gl_Position = result;
}