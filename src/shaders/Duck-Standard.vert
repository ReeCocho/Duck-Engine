#extension GL_ARB_separate_shader_objects : enable

// Vertex inputs
layout(location = 0) in vec3 IN_POSITION;
layout(location = 1) in vec2 IN_UV;
layout(location = 2) in vec3 IN_NORMAL;
layout(location = 3) in vec3 IN_TANGENT;

// Vertex ouput
out gl_PerVertex { vec4 gl_Position; };
#define OUT_POSITION gl_Position

// Material data macro
#define MATERIAL_DATA layout(set = 0, binding = 0) uniform MaterialData

// Per instance data
layout(set = 0, binding = 1) uniform VertexData
{
	layout(offset = 0)
	mat4 MODEL;
	
	layout(offset = 64)
	mat4 MVP;
};

// Output to fragment shader macro
#define FRAGMENT_IN(N) layout(location = N) out