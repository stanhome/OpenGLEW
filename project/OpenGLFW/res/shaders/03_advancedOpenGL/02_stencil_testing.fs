#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D _mainTex;


void main()
{
    FragColor = texture(_mainTex, texCoords);
}
