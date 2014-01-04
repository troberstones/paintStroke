#version 330 core
//in vec2 UV;

//uniform sampler2D topMap;
//layout(location = 0) in vec4 vertexPosition;
//layout(location = 1) in vec2 vertexUV;

in vec2 texCoord;

layout(location = 0, index = 0) out vec4 fragColor;

uniform sampler2D topMap;
void main(void)
{
    //gl_FragColor = texture2D(topMap, qt_TexCoord0.st);
    //gl_FragColor = vec4(1.f, 0.f, 0.f, 1.f);
    //gl_FragColor = vec4(UV[0], UV[1], 0.f, 1.f);
//    gl_FragColor = vec4(gl_TexCoord[0].s,gl_FragCoord.s,0,1);
    //fragColor = vec4(texCoord.s, texCoord.t, .5f, 1.f);
    fragColor = vec4(1.f,0.f,0.f,1.f);
    fragColor = texture(topMap, texCoord);
}
