#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 f_uv;

layout(set = 0, binding = 0) uniform sampler2D screen_tex;

void main() 
{
    outColor = vec4(texture(screen_tex, f_uv).rgb, 1.0);
}