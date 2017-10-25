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



Texture2D<float4> g_shadow_moments         : register(t1);

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
    float4 light_ps                   = project_p_ws(make_point_ws(r.position_ws), m_shadow_view, m_shadow_perspective).m_value;

    light_ps                          = light_ps / light_ps.w;
    float2 shadow_map_uv              = light_ps.xy * float2(0.5f, -0.5f ) + float2(0.5f, 0.5f);

    float4 optimized_moments4         = g_shadow_moments.Sample(g_linear_clamp, shadow_map_uv ).xyzw;
    float4 moments4                   = compute_deoptimized_moments(optimized_moments4);
    float  fragment_depth             = 1.0f - light_ps.z;

    float  shadow_intensity           = compute4_moment_shadow_intensity( moments4, fragment_depth);
    shadow_intensity                  = 1.0f - shadow_intensity;

    /*
    float  shadow                     = g_shadow_moments.Sample(g_linear_clamp,  light_ps.xy ).x;
    float  shadow_intensity           = shadow > light_ps.z ? 0.0f : 1.0f; 
    */

    float  ndotl                      = saturate(dot(normal_ws, sun_light_direction_ws));
    float3 ambient                    = float3(0.2f, 0.2f, 0.2f);
    
    //return float4 (shadow_intensity * ndotl * albedo * sun_light_intensity + ambient, 0.0);
    return float4 (shadow_intensity, shadow_intensity, shadow_intensity, 0.0);
}
