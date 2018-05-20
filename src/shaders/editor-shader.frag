#version 450
layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D in_texture;

layout(location = 0) in struct
{
	vec4 color;
	vec2 uv;
} f_data;

void main()
{
	out_color = f_data.color * texture(in_texture, f_data.uv.st);
}