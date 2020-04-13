#version 330 core
in vec3 fragPos;
in vec3 normal;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light {
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;


void main()
{
	/////////////
	// 1.ambient
	vec3 ambient = light.ambient * material.ambient;

	/////////////
	// 2.diffuse
	vec3 n = normalize(normal);
	vec3 lightDir = normalize(light.pos - fragPos);
	vec3 diffuse = max(dot(n, lightDir), 0.0) * material.diffuse * light.diffuse;

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

	vec3 specular = material.specular * spec * light.specular;

	/////////////
	// 4. fincal color
    FragColor = vec4((ambient + diffuse + specular), 1.0f);
}