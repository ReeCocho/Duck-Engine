#version 450
#include "Duck-Standard.vert"

MATERIAL_DATA { int unused; };

FRAGMENT_IN(0) vec2 F_UV;
FRAGMENT_IN(1) vec3 F_NORMAL;
FRAGMENT_IN(2) vec3 F_FRAG_POS;

void main() 
{
    OUT_POSITION = MVP * vec4(IN_POSITION, 1.0);
	F_UV = IN_UV;
	F_NORMAL = mat3(transpose(inverse(MODEL))) * normalize(IN_NORMAL);
	F_FRAG_POS = vec3(MODEL * vec4(IN_POSITION, 1.0));
}