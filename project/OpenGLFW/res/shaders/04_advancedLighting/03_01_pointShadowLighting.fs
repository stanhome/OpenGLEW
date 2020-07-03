#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
} fs_in;

uniform sampler2D tex_diffuse0;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool isBlinn;

uniform float far_plane;

float shadowCalculation(vec3 fragPos) {
	// get vector between fragment position and light position
	vec3 fragToLight = fragPos - lightPos;
	// the fragment to light vector to sample from the depth map
	float closestDepth = texture(depthMap, fragToLight).r;
	// it is currently in linear ragne between [0, 1], transform it back to original depth value
	closestDepth *= far_plane;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);

	float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// debug
	// fragColor = vec4(vec3(closestDepth / far_plane), 1.0);  

	return shadow;
}

void main()
{
	/////////////
	// 1.ambient
	float ambientStrength = 0.3f;
	vec3 ambient = ambientStrength * lightColor;

	/////////////
	// 2.diffuse
	float diffuseStrength = 0.5f;
	vec3 n = normalize(fs_in.normal);
	vec3 lightDir = normalize(lightPos - fs_in.fragPos);
	vec3 diffuse = max(dot(n, lightDir), 0.0) * lightColor * diffuseStrength;
	// Half Lambert 是一个视觉增强效果，没有物理依据，主要是光线找不到的阴影部分可以亮一些
	// vec3 halfLambert = diffuse * 0.5 + 0.5;
	
	/////////////
	// 3.specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);
	float spec = 0;
	if(isBlinn) {
		// Blinn-Phong, 引入了半向量的概念 h， 避免了计算反射向量 r, 提高效率
		// 也能在 θ大于 90的时候，粗糙表面下高光效果好一些
		float shininess = 64;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(halfwayDir, n), 0.0f), shininess);

	} else {
		float shininess = 16;
		// vec3 reflectDir = reflect(-lightDir, n);
		vec3 reflectDir = normalize(2 * dot(lightDir, n) * n - lightDir);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
	}

	vec3 specular = specularStrength * spec * lightColor;

	/////////////
	// 4. fincal color
	vec3 objColor = texture(tex_diffuse0, fs_in.texCoords).rgb;
	
	/////////////
	// 5. handle shadow
	float shadowVal = 1 - shadowCalculation(fs_in.fragPos);

    fragColor = vec4(objColor * (ambient + shadowVal * diffuse) + shadowVal * specular, 1.0f);
}