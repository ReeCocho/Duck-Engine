#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

out gl_PerVertex 
{
    vec4 gl_Position;
};

layout(location = 0) out vec2 f_uv;

void main() 
{
    gl_Position = vec4(in_position, 1.0);
	f_uv = in_uv;
}