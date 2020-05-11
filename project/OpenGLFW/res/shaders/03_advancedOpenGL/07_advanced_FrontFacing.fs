#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main()
{
	if (gl_FrontFacing)
		// bool true: front face
		FragColor = texture(frontTexture, texCoords);
	else
		FragColor = texture(backTexture, texCoords);
}
