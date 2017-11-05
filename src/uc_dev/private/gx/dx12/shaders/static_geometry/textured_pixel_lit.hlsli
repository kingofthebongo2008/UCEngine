#include "../default_signature.hlsli"
#include "../default_samplers.hlsli"
#include "../default_lights.hlsli"
#include "../shadows.hlsli"

#include "vector_space.hlsli"
#include "frame.hlsli"

struct interpolants
{
    float4 position       : SV_POSITION0;
    float2 uv             : texcoord0;
    float3 position_ws    : position0;
};

cbuffer shadow_parameters : register(b2)
{
    euclidean_transform_3d   m_shadow_view;
    projective_transform_3d  m_shadow_perspective;
    float4                   m_light_direction;
};

#if defined(MOMENT_SHADOW_MAPS_4)
Texture2D<float4> g_shadow_moments         : register(t1);
Texture2DArray<uint> g_blue_noise          : register(t2);
#endif

[RootSignature( MyRS1 ) ]
[earlydepthstencil]
float4 main( interpolants r ) : SV_Target0
{
    float3 dudx                       = ddx(r.position_ws);
    float3 dudy                       = ddy(r.position_ws);
    float3 normal_ws                  = normalize(cross(dudx, dudy));
    float3 sun_light_direction_ws     = m_light_direction;
    float3 sun_light_intensity        = float3(0.5, 0.5, 0.5);
    float3 albedo                     = float4(1.0f, 1.0f, 1.0f, 1.0f);
    uint   noise                      = g_blue_noise.Load( int4(0, 0, 63, 0 ) );

    float  shadow_intensity           = compute_moment4_shadow_maps( make_sampler_2d( g_shadow_moments, g_linear_clamp ), r.position_ws, make_shadow_transforms( m_shadow_view, m_shadow_perspective ) );

    float  ndotl                      = saturate(dot(normal_ws, sun_light_direction_ws));
    float3 ambient                    = 0.2f; //float3(noise / 255.0f, noise / 255.0f, noise / 255.0f);
    
    return float4 (shadow_intensity * ndotl * albedo * sun_light_intensity + ambient, 0.0);
    //return float4 (ambient, 0.0);
}


