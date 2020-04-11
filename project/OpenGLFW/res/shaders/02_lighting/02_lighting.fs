#version 330 core
in vec3 fragPos;
in vec3 normal;

out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 lightColor;

uniform vec3 lightPos;


void main()
{
	// ambient
	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 n = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 diffuse = max(dot(n, lightDir), 0.0) * lightColor;

    FragColor = vec4(objColor * (ambient + diffuse), 1.0f);
}