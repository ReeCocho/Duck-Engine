#version 450
#include "Duck-Standard.vert"

MATERIAL_DATA { int unused; };

FRAGMENT_IN(0) vec2 F_UV;
FRAGMENT_IN(1) vec3 F_NORMAL;
FRAGMENT_IN(2) vec3 F_FRAG_POS;
FRAGMENT_IN(3) mat3 F_TBN;

void main() 
{
    OUT_POSITION = MVP * vec4(IN_POSITION, 1.0);
	
	// Calculate normal and tangent
	F_NORMAL = mat3(transpose(inverse(MODEL))) * normalize(IN_NORMAL);
	vec3 tangent = vec3(MODEL * vec4(normalize(IN_TANGENT), 0.0)).xyz;
	tangent = normalize(tangent - dot(tangent, F_NORMAL) * F_NORMAL);
	F_TBN = mat3(tangent, cross(F_NORMAL, tangent), F_NORMAL);
	
	F_UV = IN_UV;
	F_FRAG_POS = vec3(MODEL * vec4(IN_POSITION, 1.0));
}