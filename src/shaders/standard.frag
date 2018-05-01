#version 450
#include "Duck-Standard.frag"

FRAGMENT_IN(0) vec2 F_UV;
FRAGMENT_IN(1) vec3 F_NORMAL;
FRAGMENT_IN(2) vec3 F_FRAG_POS;
FRAGMENT_IN(3) mat3 F_TBN;

MATERIAL_DATA 
{ 
	layout(offset = 0) float metallic; 
	layout(offset = 16) float roughness;
};

TEXTURE_IN(0) albedo_tex;
TEXTURE_IN(1) normal_tex;
TEXTURE_IN(2) metallic_tex;
TEXTURE_IN(3) roughness_tex;
TEXTURE_IN(4) ao_tex;

void main() 
{
	vec3 normal = texture(normal_tex, F_UV * 4.0).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(F_TBN * normal);

	vec3 albedo = texture(albedo_tex, F_UV * 4.0).rgb;
	float met = texture(metallic_tex, F_UV * 4.0).r;
	float rough = texture(roughness_tex, F_UV * 4.0).r;
	float ao = texture(ao_tex, F_UV * 4.0).r;
	
	vec3 li = total_lighting(F_FRAG_POS, normalize(normal), met, rough, albedo, ao);
    OUT_COLOR = vec4(li, 1.0);
}