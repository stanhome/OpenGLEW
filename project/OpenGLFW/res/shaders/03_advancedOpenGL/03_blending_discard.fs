#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D _mainTex;

void main()
{
	vec4 texColor = texture(_mainTex, texCoords);
	if(texColor.a < 0.1)
		discard;
		
	FragColor = texColor;
}
