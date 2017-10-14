#include "../default_signature.hlsli"
#include "../default_samplers.hlsli"
#include "../default_lights.hlsli"

#include "vector_space.hlsli"
#include "frame.hlsli"

struct interpolants
{
    float4 position       : SV_POSITION0;
    float2 uv             : texcoord0;
    float3 position_ws    : position0;
};

cbuffer per_draw_call : register(b2)
{
    euclidean_transform_3d   m_shadow_view;
    projective_transform_3d  m_shadow_perspective;
};

Texture2D<float4> g_shadow_moments         : register(t1);

[RootSignature( MyRS1 ) ]
float4 main( interpolants r ) : SV_Target0
{
    float3 dudx                       = ddx(r.position_ws);
    float3 dudy                       = ddy(r.position_ws);
    float3 normal_vs                  = mul(float4(normalize(cross(dudx, dudy)), 0.0f), m_view.m_value).xyz;
    float3 sun_light_direction_vs     = mul(float4(0.0,1.0,0.0, 0.0f), m_view.m_value).xyz;
    float3 sun_light_intensity        = float3(0.5, 0.5, 0.5);
    float3 albedo                     = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 light_ps                   = project_p_ws(make_point_ws(r.position_ws), m_shadow_view, m_shadow_perspective).m_value;

    light_ps                          = light_ps / light_ps.w;
    float  shadow                     = g_shadow_moments.Sample(g_linear_clamp,  light_ps.xy / 2.0f + 0.5f ).x;

    float  shadow_intensity           = shadow > light_ps.z ? 0.0f : 1.0f; 
    float  ndotl                      = saturate(dot(normal_vs, sun_light_direction_vs));
    float3 ambient                    = float3(0.2f, 0.2f, 0.2f);
    
    return float4 (shadow_intensity * ndotl * albedo * sun_light_intensity + ambient, 0.0);
}
