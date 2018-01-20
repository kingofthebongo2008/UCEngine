#ifndef __pbr_lighting_hlsli__
#define __pbr_lighting_hlsli__

#include "pbr_brdf_specular.hlsli"
#include "pbr_brdf_diffuse.hlsli"

float3 pbr_specular_brdf(float roughness, float3 n, float3 v, float3 albedo)
{
    float ndoth;
    float ldoth;

    pbr_ndoth_vdoth( ldotv, ndotl, ndotv, ndoth, ldoth );

    float alpha_g4   = pow4(roughness);

    float d         = pbr_d_ggx( alpha_g4, ndoth );
    float v         = pbr_v_smith_ggx_correlated( alpha_g4, ndotv, ndotl );
    float3 f        = pbr_fresnel_schlick(f0, ldoth);

    return d * f * v / g_pi;
}



#endif

