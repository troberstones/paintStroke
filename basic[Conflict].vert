#version 330 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 UV;

out vec2 texCoord;

void main(void)
{
    texCoord = UV;
    gl_Position = vertex;
}
