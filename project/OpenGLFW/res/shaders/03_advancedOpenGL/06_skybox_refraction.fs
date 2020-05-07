#version 330 core
in vec3 normalInWorld;
in vec3 worldPos;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube skybox;


void main()
{	
	// air to glass
	const float ratio = 1.0 / 1.52;

	vec3 I = normalize(worldPos - cameraPos);
	// vec3 R = reflect(I, normalize(normalInWorld));
	vec3 R = refract(I, normalize(normalInWorld), ratio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
