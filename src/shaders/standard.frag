#version 450
#extension GL_ARB_separate_shader_objects : enable

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

// Ins
layout(location = 0) in vec2 f_uv;

// Outs
layout(location = 0) out vec4 outColor;

// Descriptor sets
layout(set = 0, binding = 2) uniform MaterialData
{
	vec3 color;
};

layout(set = 0, binding = 3) uniform FragmentData
{
	int unused;
};

layout(set = 1, binding = 0) uniform LightingData
{
	PointLighData point_lights[DK_MAX_POINT_LIGHTS];
	uint point_light_count;
	
	DirectionalLightData directional_lights[DK_MAX_DIRECTIONAL_LIGHTS];
	uint directional_light_count;
};

layout(set = 2, binding = 0) uniform sampler2D texture_test;

void main() 
{
    outColor = vec4(texture(texture_test, f_uv).rgb, 1.0);
}