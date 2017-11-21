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

#if defined(MOMENT_SHADOW_MAPS_4)

cbuffer shadow_parameters : register(b2)
{
    euclidean_transform_3d   m_shadow_view;
    projective_transform_3d  m_shadow_perspective;
    float4                   m_light_direction;
};

Texture2D<float4> g_shadow_moments         : register(t1);
Texture2DArray<uint> g_blue_noise          : register(t2);

[RootSignature( MyRS1 ) ]
[earlydepthstencil]
float4 main( interpolants r ) : SV_Target0
{
    float3 dudx = ddx(r.position_ws);
    float3 dudy = ddy(r.position_ws);
    float3 normal_ws = normalize(cross(dudx, dudy));
    float3 sun_light_direction_ws = m_light_direction.xyz;
    float3 sun_light_intensity = float3(0.5, 0.5, 0.5);
    float3 albedo = float3(1.0f, 1.0f, 1.0f);
    uint   noise = g_blue_noise.Load(int4(0, 0, 63, 0));

    float  shadow_intensity = compute_moment4_shadow_maps(make_sampler_moments_64(g_shadow_moments, g_linear_clamp), r.position_ws, make_moments_64_context(make_shadow_transforms(m_shadow_view, m_shadow_perspective)));

    float  ndotl = saturate(dot(normal_ws, sun_light_direction_ws));
    float3 ambient = 0.2f; //float3(noise / 255.0f, noise / 255.0f, noise / 255.0f);

    return float4 (shadow_intensity * ndotl * albedo * sun_light_intensity + ambient, 0.0);
    //return float4 (ambient, 0.0);
}
#endif

#if defined(NON_LINEAR_MOMENT_SHADOW_MAPS_32)

cbuffer shadow_parameters : register(b2)
{
    euclidean_transform_3d   m_shadow_view;
    euclidean_transform_3d   m_main_view;

    projective_transform_3d  m_shadow_perspective;
    projective_transform_3d  m_main_perspective;

    float4                   m_view_port_transform;
    float4                   m_light_direction;

    uint3                    m_randomness;  // xz in the blue noise texture, y blue noise index itself
    uint                     m_pad0;

    uint2                    m_shadow_buffer_size;
    uint2                    m_pad1;
};

Texture2D<uint>         g_shadow_moments   	  : register(t1);
Texture2DArray<float2>  g_blue_noise          : register(t2);

int2 compute_view_port_pixel_index(float2 projection_space_position, float4 viewport_transform)
{
    return int2(mad(projection_space_position.xy, viewport_transform.xy, viewport_transform.zw));
}

[RootSignature( MyRS1 ) ]
[earlydepthstencil]
float4 main( interpolants r ) : SV_Target0
{
        float3 dudx                                 = ddx(r.position_ws);
        float3 dudy                                 = ddy(r.position_ws);
        float3 normal_ws                            = normalize(cross(dudx, dudy));
        float3 sun_light_direction_ws               = m_light_direction.xyz;
        float3 sun_light_intensity                  = float3(0.5, 0.5, 0.5);
        float3 albedo                               = float3(1.0f, 1.0f, 1.0f);
        float2 noise                                = g_blue_noise.Load(int4(0, 0, 63, 0));

        point_ps position_ps                        = project_p_ws(make_point_ws(r.position_ws), m_main_view, m_main_perspective);

        non_linear_moments_32_context               ctx;

        ctx.m_shadow_transforms.m_view              = m_shadow_view;
        ctx.m_shadow_transforms.m_perspective       = m_shadow_perspective;

        ctx.m_main_view_transforms.m_view           = m_shadow_view;
        ctx.m_main_view_transforms.m_perspective    = m_shadow_perspective;

        ctx.m_randomness                            = m_randomness;
        ctx.m_shadow_buffer_size                    = m_shadow_buffer_size;
        ctx.m_view_port_pixel_index                 = compute_view_port_pixel_index(position_ps.m_value.xy, m_view_port_transform);

        float  shadow_intensity                     = compute_nonlinear_moment4_shadow_maps_32(make_sampler_non_linear_moments_32(g_shadow_moments, g_blue_noise), r.position_ws, ctx);

        float  ndotl                                = saturate(dot(normal_ws, sun_light_direction_ws));
        float3 ambient                              = 0.2f; //float3(noise / 255.0f, noise / 255.0f, noise / 255.0f);

        return float4 (shadow_intensity * ndotl * albedo * sun_light_intensity + ambient, 0.0);
        //return float4 (ambient, 0.0);
}
#endif



