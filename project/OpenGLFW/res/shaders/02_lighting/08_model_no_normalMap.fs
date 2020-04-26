#version 330 core
in vec3 fragPos;
in vec2 texCoords;
in vec3 normal;

out vec4 FragColor;

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

uniform sampler2D tex_diffuse0;
uniform sampler2D tex_specular0;
uniform float mat_shininess;
uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLight;


// fucntion prototypes
vec3 calcDirLight(DirLight light, vec3 n, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 n, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 n = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	// phase 1: directional lighting
	vec3 result = calcDirLight(dirLight, n, viewDir);

	// phase 2: point lights
	result += calcPointLight(pointLight, n, fragPos, viewDir);

	/////////////
	// 4. fincal color
	// FragColor = texture(tex_diffuse0, texCoords);
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
	vec3 reflectDir = normalize(2 * dot(lightDir, n) * n - lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat_shininess);

	//combine results
	vec3 diffuseMap = vec3(texture(tex_diffuse0, texCoords));
	vec3 specularMap = vec3(texture(tex_specular0, texCoords));
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
	vec3 reflectDir = normalize(2 * dot(lightDir, n) * n - lightDir);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), mat_shininess);

	// atteuation 衰减
	float distance = length(light.pos - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//combine results
	vec3 diffuseMap = vec3(texture(tex_diffuse0, texCoords));
	vec3 specularMap = vec3(texture(tex_specular0, texCoords));
	vec3 ambient = light.ambient * diffuseMap;
	vec3 diffuse = light.diffuse * diff * diffuseMap;
	vec3 specular = light.specular * spec * specularMap;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}