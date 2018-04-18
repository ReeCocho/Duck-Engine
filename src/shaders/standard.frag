#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform MaterialData
{
	vec3 color;
};

layout(set = 0, binding = 3) uniform FragmentData
{
	int unused;
};

layout(location = 0) in vec2 f_uv;

layout(set = 1, binding = 0) uniform sampler2D texture_test;

void main() 
{
    outColor = vec4(texture(texture_test, f_uv).rgb, 1.0);
}