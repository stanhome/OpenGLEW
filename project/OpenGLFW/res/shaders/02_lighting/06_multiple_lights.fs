#version 330 core
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;


struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 pos;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 pos;
	vec3 direction;
	float cutoffCos;
	float outerCutoffCos;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NUMBER_POINT_LIGHTS 4

uniform vec3 viewPos;
uniform Material material;

uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];
uniform SpotLight spotLight;


// fucntion prototypes
vec3 calcDirLight(DirLight light, vec3 n, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 n, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 n, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 n = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	// phase 1: directional lighting
	vec3 result = calcDirLight(dirLight, n, viewDir);

	// phase 2: point lights
	for (int i = 0; i < NUMBER_POINT_LIGHTS; i++)
		result += calcPointLight(pointLights[i], n, fragPos, viewDir);

	// phase 3: spot light
	result += calcSpotLight(spotLight, n, fragPos, viewDir);

	/////////////
	// 4. fincal color
    FragColor = vec4(result, 1.0f);
}

// calculates the color when using a driectional light.
vec3 calcDirLight(DirLight light, vec3 n, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	// diffuse shading
	float diff = max(dot(n, lightDir), 0.0);

	// specular shading
	// vec3 reflectDir = reflect(-lightDir, normal);
	vec3 reflectDir = normalize(2 * dot(lightDir, normal) * normal - lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	//combine results
	vec3 diffuseMap = vec3(texture(material.diffuse, texCoords));
	vec3 specularMap = vec3(texture(material.specular, texCoords));
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;

	return (ambient + diffuse + specular);
}

// calcualtes the color when using a point light
vec3 calcPointLight(PointLight light, vec3 n, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.pos - fragPos);

	// diffuse shading
	float diff = max(dot(n, lightDir), 0.0);

	// specular shading
	// vec3 reflectDir = reflect(-lightDir, normal);
	vec3 reflectDir = normalize(2 * dot(lightDir, normal) * normal - lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	// atteuation 衰减
	float distance = length(light.pos - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//combine results
	vec3 diffuseMap = vec3(texture(material.diffuse, texCoords));
	vec3 specularMap = vec3(texture(material.specular, texCoords));
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

// calcualtes the color when using a spot light.
vec3 calcSpotLight(SpotLight light, vec3 n, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.pos - fragPos);
	vec3 spotDir = normalize(-light.direction);

	// diffuse shading
	float diff = max(dot(n, lightDir), 0.0);

	// specular shading
	// vec3 reflectDir = reflect(-lightDir, normal);
	vec3 reflectDir = normalize(2 * dot(lightDir, normal) * normal - lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	// atteuation 衰减
	float distance = length(light.pos - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// spotlight intensity
	float theta = dot(lightDir, spotDir);
	float epsilon = (light.cutoffCos - light.outerCutoffCos);
	float intensity = clamp((theta - light.outerCutoffCos) / epsilon, 0.0, 1.0);


	//combine results
	vec3 diffuseMap = vec3(texture(material.diffuse, texCoords));
	vec3 specularMap = vec3(texture(material.specular, texCoords));
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}