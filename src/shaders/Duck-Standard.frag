#extension GL_ARB_separate_shader_objects : enable

// Light macros
#define DK_MAX_POINT_LIGHTS 64
#define DK_MAX_DIRECTIONAL_LIGHTS 16

struct PointLighData
{
	/** Position. */
	vec4 position;
	
	/** Color. */
	vec4 color;
	
	/** Range. */
	float range;
	
	/** Intensity. */
	float intensity;
};

struct DirectionalLightData
{
	/** Direction. */
	vec4 direction;
	
	/** Color. */
	vec4 color;
	
	/** Intensity */
	float intensity;
};

struct CalculationData
{
	/** Light */
	vec3 direction;
	vec3 light_color;
	float attenuation;
	
	/** Material */
	float metallic;
	float roughness;
	vec3 albedo;
	vec3 reflectance;
	
	/** Misc */
	vec3 normal;
	vec3 view_direction;
};

// Input to fragment shader macro
#define FRAGMENT_IN(N) layout(location = N) in

// Outs
layout(location = 0) out vec4 OUT_COLOR;

// Per instance data
layout(set = 0, binding = 3) uniform FragmentData
{
	int unused;
};

// Lighting data
layout(set = 1, binding = 0) uniform LightingData
{
	layout(offset = 0) 
	PointLighData point_lights[DK_MAX_POINT_LIGHTS];
	
	layout(offset = (DK_MAX_POINT_LIGHTS * 40)) 
	uint point_light_count;
	
	layout(offset = (DK_MAX_POINT_LIGHTS * 40) + 16) 
	DirectionalLightData directional_lights[DK_MAX_DIRECTIONAL_LIGHTS];
	
	layout(offset = (DK_MAX_POINT_LIGHTS * 40) + (DK_MAX_DIRECTIONAL_LIGHTS * 36) + 16)
	uint directional_light_count;
	
	layout(offset = (DK_MAX_POINT_LIGHTS * 40) + (DK_MAX_DIRECTIONAL_LIGHTS * 36) + 32)
	vec4 ambient;
	
	layout(offset = (DK_MAX_POINT_LIGHTS * 40) + (DK_MAX_DIRECTIONAL_LIGHTS * 36) + 48)
	vec4 camera_position;
	
} lighting_data;

// Material data macro
#define MATERIAL_DATA layout(set = 0, binding = 3) uniform MaterialData

// Texture macro
#define TEXTURE_IN(N) layout(set = 2, binding = N) uniform sampler2D

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 lighting(CalculationData data)
{
	// Calculate per-light radiance
	vec3 L = normalize(data.direction);
	vec3 H = normalize(data.view_direction + L);
	vec3 radiance = data.light_color * data.attenuation;
	
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(data.normal, H, data.roughness);
	float G = GeometrySmith(data.normal, data.view_direction, L, data.roughness);
	vec3 F = fresnelSchlick(clamp(dot(H, data.view_direction), 0.0, 1.0), data.reflectance);
	
	vec3 nominator = NDF * G * F;
	float denominator = 4.0 * max(dot(data.normal, data.view_direction), 0.0) * max(dot(data.normal, L), 0.0);
	vec3 specular = nominator / max(denominator, 0.001);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - data.metallic;
	
	float NdotL = max(dot(data.normal, L), 0.0);
	
	return (kD * data.albedo / PI + specular) * radiance * NdotL;
}

vec3 total_lighting(vec3 position, vec3 normal, float metallic, float roughness, vec3 albedo, float ao)
{
	// View direction
	vec3 view_dir = normalize(lighting_data.camera_position.xyz - position);
	
	// Reflectance
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	// Total color
	vec3 total = vec3(0.0);
	
	// Directional lights
	for(uint i = 0; i < lighting_data.directional_light_count; ++i)
	{
		CalculationData data;
		data.direction = -normalize(lighting_data.directional_lights[i].direction.xyz);
		data.light_color = lighting_data.directional_lights[i].color.rgb * lighting_data.directional_lights[i].intensity;
		data.attenuation = 1.0;
		data.metallic = metallic;
		data.roughness = roughness;
		data.albedo = albedo;
		data.reflectance = F0;
		data.normal = normal;
		data.view_direction = view_dir;
	
		total += lighting(data);
	}
	
	// // Ambient and AO
	total += albedo * lighting_data.ambient.rgb * lighting_data.ambient.w * ao;
	 
	// HDR tonemapping
	// total = total / (total + vec3(1.0));
	
	// Gama correct
	// total = pow(total, vec3(1.0/2.2));
	
	return total;
}