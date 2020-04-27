#version 330 core
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D _mainTex;

float near = 0.1;
float far = 100.0f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{
	// non-linear
	// FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);

	// linear depth
	// divide by far for demonstration
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; 
    // FragColor = vec4(vec3(depth), 1.0);

    FragColor = texture(_mainTex, texCoords);
}
