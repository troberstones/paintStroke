#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

uniform mat4 MVP;

void main(void)
{
    //gl_Position = ftransform();
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    //gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
    UV = vertexUV;
}
