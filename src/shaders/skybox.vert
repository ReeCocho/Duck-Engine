#version 450
#include "Duck-Standard.vert"

MATERIAL_DATA { int unused; };

FRAGMENT_IN(0) vec3 F_UVW;

void main() 
{
    OUT_POSITION = MVP * vec4(IN_POSITION, 1.0);
	F_UVW = IN_POSITION;
}