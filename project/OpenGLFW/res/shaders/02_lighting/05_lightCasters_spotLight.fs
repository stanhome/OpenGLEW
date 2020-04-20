#version 330 core
in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

//Spot light
struct Light {
	vec3 pos;
	vec3 direction;
	float cutoffCos;


	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
uniform Light light;


void main()
{
	vec3 diffuseMap = vec3(texture(material.diffuse, texCoords));
	vec3 lightDir = normalize(light.pos - fragPos);
	vec3 spotDir = normalize(-light.direction);

	// check if lighting is inside the spotlight cone.
	float theta = dot(lightDir, spotDir);
	if (theta > light.cutoffCos) {

		/////////////
		// 1.ambient
		vec3 ambient = light.ambient * diffuseMap;

		/////////////
		// 2.diffuse
		vec3 n = normalize(normal);
		vec3 diffuse = max(dot(n, lightDir), 0.0) * diffuseMap * light.diffuse;

		// Half Lambert 是一个视觉增强效果，没有物理依据，主要是光线找不到的阴影部分可以亮一些
		// vec3 halfLambert = diffuse * 0.5 + 0.5;
		

		/////////////
		// 3.specular
		vec3 viewDir = normalize(viewPos - fragPos);
		// vec3 reflectDir = reflect(-lightDir, normal);
		vec3 reflectDir = normalize(2 * dot(lightDir, normal) * normal - lightDir);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

		// Blinn-Phong, 引入了半向量的概念 h， 避免了计算反射向量 r, 提高效率
		// vec3 h = normalize(viewDir + lightDir);
		// spec = pow(max(dot(h, normal), 0.0f), shininess);

		vec3 specular = vec3(texture(material.specular, texCoords)) * spec * light.specular;

		// Point light
		float distance = length(light.pos - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		//衰减
		// remove attenuation from ambient, as otherwise at large distances the light would be darker inside than
		// outside the spotlight due the ambient term in the else branche
		// ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;

		/////////////
		// 4. fincal color
	    FragColor = vec4((ambient + diffuse + specular), 1.0f);
	} else {
		// else, use ambient light so scene isn't completely dark outside the spotlight.
		FragColor = vec4(light.ambient * diffuseMap, 1.0f);
	}

}