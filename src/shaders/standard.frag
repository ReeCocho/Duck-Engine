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

layout(location = 0) in vec3 o_color;

void main() 
{
    outColor = vec4((o_color / 2.0) + 0.5, 1.0);
}