#ifndef __shade_hlsli__
#define __shade_hlsli__

#include "../color/color.hlsli"
#include "../lighting/pbr_diffuse.hlsli"
#include "../lighting/pbr_specular.hlsli"

color_rec_709 shade_direct_light( float3 n, float3 l, float3 v, float roughness, float metallic, float3 albedo, color_rec_709 light_color )
{
    float ldotv = saturate(dot(l,v));
    float ndotl = saturate(dot(n,l));
    float ndotv = saturate(dot(n,v));

    const float3 c_diff = lerp(albedo, float3(0, 0, 0), metallic)       * ambientOcclusion;
    const float3 c_spec = lerp(kSpecularCoefficient, albedo, metallic)  * ambientOcclusion;
    
    float3  f0_factor   = lerp(f0, 0.04, metalic);
    float3 diffuse      = pbr_diffuse_brdf(roughness, albedo, ldotv, ndotl, ndotv);
    float3 specular     = pbr_diffuse_brdf(roughness, f0, ldotv, ndotl, ndotv);

    return ( diffuse + specular ) * ndotl * light_color;
}





#endif
