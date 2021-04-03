#version 420 core

in vec3 Texcoord;

out vec4 outColor;

uniform samplerCube texSmplr;

void main()
{    
    outColor = texture(texSmplr, Texcoord) * 0.5f;
}
