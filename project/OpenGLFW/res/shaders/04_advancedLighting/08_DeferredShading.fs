#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
	vec3 pos;
	vec3 color;
	float linear;
	float quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{
	// retrieve data from G-buffer
	vec3 fragPos = texture(gPosition, texCoords).rgb;
	vec3 n = texture(gNormal, texCoords).rgb;
	vec4 albedoSpec = texture(gAlbedoSpec, texCoords);
	vec3 albedo = albedoSpec.rgb;
	float spec = albedoSpec.a;


	// then calculate lighting as usual
	vec3 lighting = albedo * 0.1f; // hardcoded ambient component
	vec3 viewDir = normalize(viewPos - fragPos);
	const float shininess = 16.0;
	for (int i = 0; i < NR_LIGHTS; ++i) {
		Light light = lights[i];

		// diffuse
		vec3 lightDir = normalize(lights[i].pos - fragPos);
		vec3 diffuse = max(dot(n, lightDir), 0.0) * albedo * light.color;

		// specular
		vec3 halfwayDir = normalize(lightDir + viewDir);
		vec3 specular = pow(max(dot(halfwayDir, n), 0.0f), shininess) * spec * light.color;
		// attenuation
		float distance = length(light.pos - fragPos);
		float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
		diffuse *= attenuation;
		specular *= attenuation;

		lighting += diffuse + specular;
	}

    fragColor = vec4(lighting, 1.0f);
}