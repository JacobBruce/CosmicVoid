#version 420 core

layout (location = 0) in vec3 position;

out vec3 Texcoord;

uniform mat4 mvp;

void main()
{
    Texcoord = vec3(position.x, -position.y, position.z);
    gl_Position = mvp * vec4(position, 1.0);
}
