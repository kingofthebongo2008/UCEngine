#ifndef __shadows_hlsli__
#define __shadows_hlsli__

#include "shadows/moment_shadow_maps_compute.hlsli"
#include "static_geometry/vector_space.hlsli"

struct sampler_moments_64
{
    Texture2D<float4>  m_texture;
    SamplerState	   m_sampler;
};

struct sampler_non_linear_moments_64
{
    Texture2D<float4>       m_texture;
    Texture2DArray<float2>  m_blue_noise;
};

struct sampler_non_linear_moments_32
{
    Texture2D<uint>         m_texture;
    Texture2DArray<float2>  m_blue_noise;
};

struct shadow_transforms
{
    euclidean_transform_3d   m_shadow_view;
    projective_transform_3d  m_shadow_perspective;
};

struct moments_64_context
{
    shadow_transforms m_transforms;
};

struct non_linear_moments_64_context
{
    shadow_transforms m_transforms;
    uint4             m_randomness;
};

sampler_moments_64 make_sampler_moments_64( Texture2D<float4> t, SamplerState s )
{
   sampler_moments_64 r = { t, s };
   return r;
}

sampler_non_linear_moments_64 make_sampler_non_linear_moments_64(Texture2D<float4> s, Texture2DArray<float2> noise)
{
    sampler_non_linear_moments_64 r = { s, noise };
    return r;
}

sampler_non_linear_moments_32 make_sampler_non_linear_moments_32(Texture2D<uint> s, Texture2DArray<float2> noise)
{
    sampler_non_linear_moments_32 r = { s, noise };
    return r;
}

shadow_transforms make_shadow_transforms( euclidean_transform_3d v, projective_transform_3d p )
{
    shadow_transforms r = { v, p };
    return r;
}

moments_64_context make_moments_64_context(shadow_transforms t)
{
    moments_64_context r = {t};
    return r;
}

float compute_moment4_shadow_maps(sampler_moments_64 moments, float3 position_ws, moments_64_context ctx )
{
    float4 light_ps                   = project_p_ws(make_point_ws(position_ws), ctx.m_transforms.m_shadow_view, ctx.m_transforms.m_shadow_perspective).m_value;
    light_ps                          = light_ps / light_ps.w;
    float2 shadow_map_uv              = light_ps.xy * float2(0.5f, -0.5f ) + float2(0.5f, 0.5f);

    float4 optimized_moments4         = moments.m_texture.Sample(moments.m_sampler, shadow_map_uv ).xyzw;
    float4 moments4                   = compute_deoptimized_moments(optimized_moments4);
    float  fragment_depth             = 1.0f - light_ps.z; //shadows are stored in 1-0, not 0-1

    float  shadow_intensity           = compute4_moment_shadow_intensity( moments4, fragment_depth);
    shadow_intensity                  = 1.0f - shadow_intensity;

    return shadow_intensity;
}

float compute_nonlinear_moment4_shadow_maps_64(sampler_moments_64 moments, float3 position_ws, shadow_transforms t)
{
    float4 light_ps = project_p_ws(make_point_ws(position_ws), t.m_shadow_view, t.m_shadow_perspective).m_value;
    light_ps = light_ps / light_ps.w;
    float2 shadow_map_uv = light_ps.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    float4 optimized_moments4   = moments.m_texture.Sample(moments.m_sampler, shadow_map_uv).xyzw;
    float4 moments4             = compute_deoptimized_moments(optimized_moments4);
    float  fragment_depth       = 1.0f - light_ps.z; //shadows are stored in 1-0, not 0-1

    float  shadow_intensity = compute4_moment_shadow_intensity(moments4, fragment_depth);
    shadow_intensity = 1.0f - shadow_intensity;

    return shadow_intensity;
}

    /*
    float  shadow                     = g_shadow_moments.Sample(g_linear_clamp,  light_ps.xy ).x;
    float  shadow_intensity           = shadow > light_ps.z ? 0.0f : 1.0f; 
    */



#endif
