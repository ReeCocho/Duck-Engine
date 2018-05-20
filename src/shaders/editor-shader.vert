#version 450
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;

layout(push_constant) uniform uPushConstant
{
	vec2 scale;
	vec2 translate;
} pc;

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out struct
{
	vec4 color;
	vec2 uv;
} f_data;

void main()
{
	f_data.color = in_color;
	f_data.uv = in_uv;
	gl_Position = vec4(in_pos * pc.scale + pc.translate, 0, 1);
}