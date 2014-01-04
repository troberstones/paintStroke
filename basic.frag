#version 330 core
in vec2 UV;

uniform sampler2D topMap;

void main(void)
{
    //gl_FragColor = texture2D(topMap, qt_TexCoord0.st);
    //gl_FragColor = vec4(1.f, 0.f, 0.f, 1.f);
    gl_FragColor = vec4(uv.s, uv.t, 0.f, 1.f);
}
