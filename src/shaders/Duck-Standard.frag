#extension GL_ARB_separate_shader_objects : enable



// Input to fragment shader macro
#define FRAGMENT_IN(N) layout(location = N) in

// Outs
layout(location = 0) out vec4 OUT_COLOR;

// Per instance data
layout(set = 0, binding = 3) uniform FragmentData
{
	int UNUSED_VARIABLE;
};

// Material data macro
#define MATERIAL_DATA layout(set = 0, binding = 3) uniform MaterialData

// Texture macro
#define TEXTURE_IN(N) layout(set = 2, binding = N) uniform sampler2D

// Cube map macro
#define CUBE_MAP_IN(N) layout(set = 2, binding = N) uniform samplerCube

const float PI = 3.14159265359;



struct PointLighData
{
	/** Position. */
	vec4 position;
	
	/** Color. */
	vec4 color;
};

struct DirectionalLightData
{
	/** Direction. */
	vec4 direction;
	
	/** Color. */
	vec4 color;
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

// Lighting data
layout(std430, set = 1, binding = 0) buffer PointLights
{
	uint light_count;
	PointLighData data[];
} point_lights;

layout(std430, set = 1, binding = 1) buffer DirectionalLights
{
	uint light_count;
	DirectionalLightData data[];
} directional_lights;

layout(set = 1, binding = 2) uniform LightingData
{
	layout(offset = 0)
	vec4 ambient;
	
	layout(offset = 16)
	vec4 camera_position;
} lighting_data;

float distribution_GGX(vec3 N, vec3 H, float roughness)
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

float geometry_schlick_GGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_GGX(NdotV, roughness);
    float ggx1 = geometry_schlick_GGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
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
	float NDF = distribution_GGX(data.normal, H, data.roughness);
	float G = geometry_smith(data.normal, data.view_direction, L, data.roughness);
	vec3 F = fresnel_schlick(max(dot(H, data.view_direction), 0.0), data.reflectance);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - data.metallic;
	
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(data.normal, data.view_direction), 0.0) * max(dot(data.normal, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);
	
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
	for(uint i = 0; i < directional_lights.light_count; ++i)
	{
		CalculationData data;
		data.direction = -normalize(directional_lights.data[i].direction.xyz);
		data.light_color = directional_lights.data[i].color.rgb * directional_lights.data[i].color.a;
		data.attenuation = 1.0;
		data.metallic = metallic;
		data.roughness = roughness;
		data.albedo = albedo;
		data.reflectance = F0;
		data.normal = normal;
		data.view_direction = view_dir;
	
		total += lighting(data);
	}
	
	// Point lights
	for(uint i = 0; i < point_lights.light_count; ++i)
	{
		vec3 pos_diff = point_lights.data[i].position.xyz - position;
		float dist = length(pos_diff);
		
		if(dist < point_lights.data[i].position.w)
		{	
			vec3 direction = normalize(pos_diff);
			float atten = clamp(1.0 - (dist/point_lights.data[i].position.w), 0.0, 1.0);
			atten *= atten;
		
			CalculationData data;
			data.direction = direction;
			data.light_color = point_lights.data[i].color.rgb * point_lights.data[i].color.a;
			data.attenuation = atten;
			data.metallic = metallic;
			data.roughness = roughness;
			data.albedo = albedo;
			data.reflectance = F0;
			data.normal = normal;
			data.view_direction = view_dir;
		
			total += lighting(data);
		}
	}
	
	// // Ambient and AO
	total += albedo * lighting_data.ambient.rgb * lighting_data.ambient.w * ao;
	 
	// HDR tonemapping
	total = total / (total + vec3(1.0));
	
	// Gama correct
	total = pow(total, vec3(1.0/2.2));
	
	return total;
}