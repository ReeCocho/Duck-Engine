#version 450
#include "Duck-Standard.frag"

FRAGMENT_IN(0) vec2 F_UV;
FRAGMENT_IN(1) vec3 F_NORMAL;
FRAGMENT_IN(2) vec3 F_FRAG_POS;

MATERIAL_DATA { int unused_val; };

TEXTURE_IN(0) texture_test;

void main() 
{
	vec3 albedo = texture(texture_test, F_UV).rgb;
	vec3 li = total_lighting(F_FRAG_POS, normalize(F_NORMAL), 0.0, 0.0, albedo, 1.0);
    OUT_COLOR = vec4(li, 1.0);
}