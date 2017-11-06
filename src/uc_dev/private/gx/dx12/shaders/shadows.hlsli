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

    euclidean_transform_3d  shadow_view()        { return m_transforms.m_shadow_view;}
    projective_transform_3d shadow_perspective() { return m_transforms.m_shadow_perspective;}
};

struct size_2d_uint
{
    uint m_width;
    uint m_height;

    uint width()  { return m_width; }
    uint height() { return m_height;  }
};

struct non_linear_moments_64_context
{
    shadow_transforms       m_transforms;
    uint2                   m_randomness;
    size_2d_uint            m_shadow_buffer_size;

    euclidean_transform_3d  shadow_view()           { return m_transforms.m_shadow_view; }
    projective_transform_3d shadow_perspective()    { return m_transforms.m_shadow_perspective; }

    uint2                   randomness()            { return m_randomness; }
    size_2d_uint            shadow_buffer_size()    { return m_shadow_buffer_size; }
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

struct shadow_info
{
    float2 m_shadow_buffer_uv;
    float  m_fragment_depth;

    float  fragment_depth() { return m_fragment_depth; }
    float2 shadow_buffer_uv() { return m_shadow_buffer_uv; }
};

shadow_info compute_fragment_depth(float3 position_ws, euclidean_transform_3d shadow_view, projective_transform_3d shadow_perspective)
{
    float4 light_ps         = project_p_ws(make_point_ws(position_ws), shadow_view, shadow_perspective).m_value;
    light_ps                = light_ps / light_ps.w;
    float2 shadow_map_uv    = mad(light_ps.xy, float2(0.5f, -0.5f), float2(0.5f, 0.5f));
    float  fragment_depth   = 1.0f - light_ps.z; //shadows are stored in 1-0, not 0-1
    shadow_info r           = { shadow_map_uv, fragment_depth };
    return r;
}

float compute_moment4_shadow_maps(sampler_moments_64 moments, float3 position_ws, moments_64_context ctx )
{
    shadow_info s                     = compute_fragment_depth(position_ws, ctx.shadow_view(), ctx.shadow_perspective());
    float4 optimized_moments4         = moments.m_texture.Sample( moments.m_sampler, s.shadow_buffer_uv() ) .xyzw;
    float4 moments4                   = compute_deoptimized_moments(optimized_moments4);

    float  shadow_intensity           = compute4_moment_shadow_intensity( moments4, s.fragment_depth());
    shadow_intensity                  = 1.0f - shadow_intensity;
    return shadow_intensity;
}

float compute_nonlinear_moment4_shadow_maps_64(sampler_moments_64 moments, float3 position_ws, non_linear_moments_64_context ctx)
{
    shadow_info s               = compute_fragment_depth(position_ws, ctx.shadow_view(), ctx.shadow_perspective());

    float4 optimized_moments4   = moments.m_texture.Sample(moments.m_sampler, s.shadow_buffer_uv() ).xyzw;
    float4 moments4             = compute_deoptimized_moments(optimized_moments4);

    float  shadow_intensity     = compute4_moment_shadow_intensity(moments4, s.fragment_depth());
    shadow_intensity            = 1.0f - shadow_intensity;

    return shadow_intensity;
}

    /*
    float  shadow                     = g_shadow_moments.Sample(g_linear_clamp,  light_ps.xy ).x;
    float  shadow_intensity           = shadow > light_ps.z ? 0.0f : 1.0f; 
    */



#endif
