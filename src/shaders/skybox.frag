#version 450
#include "Duck-Standard.frag"

FRAGMENT_IN(0) vec3 F_UVW;

MATERIAL_DATA { layout(offset = 0) int unused; };

CUBE_MAP_IN(0) cube_map_tex;

void main() 
{
    OUT_COLOR = vec4(texture(cube_map_tex, F_UVW).rgb, 1.0);
}