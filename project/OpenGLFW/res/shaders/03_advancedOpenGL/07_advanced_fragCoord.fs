#version 330 core
// in vec2 texCoords;

out vec4 FragColor;

void main()
{
	//gl_FragCoord
	// windows-space, z is depth value
	if (gl_FragCoord.x < 400)
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
