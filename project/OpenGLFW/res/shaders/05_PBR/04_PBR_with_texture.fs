#version 330 core
out vec4 fragColor;

in VS_OUT {
    vec3 worldPos;
    vec2 texCoords;
    mat3 TBN;
} fs_in;

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// material parameters
uniform vec3 albedoProp;
uniform float metallicProp;
uniform float roughnessProp;
uniform float ao;

uniform sampler2D tex_diffuse0; // albedo
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

// lights
uniform vec3 lightPosArr[4];
uniform vec3 lightColorArr[4];

uniform vec3 viewPos;

const float PI = 3.14159265359;

// normal distribution
float distributionGGX(vec3 n, vec3 h, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(n, h), 0.0);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH2 * (a2 - 1.0) + 1.0);
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

// geometry function
float geometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float numerator = NdotV;
	float denominator = NdotV * (1 - k) + k;

	return numerator / denominator;
}
float geometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	float NdotV = max(dot(n, v), 0.0);
	float NdotL = max(dot(n, l), 0.0);
	float ggx1 = geometrySchlickGGX(NdotV, roughness);
	float ggx2 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Fresnel
vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 f0, float roughness) {
	return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0 - cosTheta, 5.0);
}

// Cook-Torrance BRDF
vec3 BRDF(vec3 n, vec3 v, vec3 l, float roughness, float metallic, vec3 F0, vec3 albedoColor) {
	vec3 h = normalize(v + l);
	
	float NDF = distributionGGX(n, h, roughness);
	float G = geometrySmith(n, v, l, roughness);
	vec3 F = fresnelSchlick(clamp(dot(n, v), 0.0, 1.0), F0);

	// diffuse term
	vec3 diffuse = albedoColor / PI;

	// specular term
	vec3 numerator = NDF * G * F;
	float denominator = 4 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);

	//ratio
	vec3 kS = F; // kS is equal to Fresnel
	// for energy convervation, the diffuse and specular light can't be above 1.0(unless the surface emits light);
	// to preserve this relationship
	// the diffuse component(kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;

	// multiply kD by the inverse metalness such that only non-metals have diffuse lighting.
	// or a linear blend if partly metal(pure metals have no diffuse light).
	kD *= (1.0 - metallic);

	// Note that we already multiplied the BRDF by the Fresnel(kS), so we won't multiply by kS again.
	return kD * diffuse + specular;
}


void main()
{
	vec3 albedo = pow(texture(tex_diffuse0, fs_in.texCoords).rgb, vec3(2.2)) * albedoProp;
	float metallic = texture(metallicMap, fs_in.texCoords).r * metallicProp;
	float roughness = texture(roughnessMap, fs_in.texCoords).r * roughnessProp;
	vec3 normalMap = texture(normalMap, fs_in.texCoords).rgb;


	vec3 n = normalize(fs_in.TBN * normalMap);
	vec3 v = normalize(viewPos - fs_in.worldPos);
	vec3 r = reflect(-v, n);

	// calculate reflectance at normal incidence;
	// if dia-electric (like plastic) use F0 of 0.04
	// if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; ++i) {
		// calculate per-light radiance
		vec3 l = lightPosArr[i] - fs_in.worldPos;
		float lightDistance = length(l);
		float attenuation = 1.0 / (lightDistance * lightDistance);
		vec3 radiance = lightColorArr[i] * attenuation;

		// Cook-Torrance BRDF
		l = normalize(l);
		vec3 BRDFVal = BRDF(n, v, l, roughness, metallic, f0, albedo);

		// scale light by NdotL
		float NdotL = max(dot(n, l), 0.0);

		// add to outgoing radiance Lo
		Lo +=  BRDFVal * radiance * NdotL;
	}

	// ambient lighting(we now use IBL as the ambient term).
	// vec3 kS = fresnelSchlick(max(dot(n, v), 0.0), f0);
	vec3 F = fresnelSchlickRoughness(max(dot(n, v), 0.0), f0, roughness);

	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= (1.0 - metallic);

	vec3 irradiance = texture(irradianceMap, n).rgb;
	vec3 diffuse = irradiance * albedo;

	// sample both the pre-filter map and the BRDF LUT and combine them together as per the Split-Sum approximation to get the IBL specular part.
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, r, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(n, v), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	vec3 ambient =  (kD * diffuse + specular) * ao;
	// vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = ambient + Lo;

	// HDR tone mapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0f);
}