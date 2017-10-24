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

cbuffer shadow_parameters : register(b2)
{
    euclidean_transform_3d   m_shadow_view;
    projective_transform_3d  m_shadow_perspective;
    float4                   m_light_direction;
};


/*! Given a sampled value from a four-moment shadow map and a computed shadow map
depth for a point at the same location this function outputs 1.0, if the fragment
is in shadow 0.0f, if the fragment is lit and an intermediate value for partial
shadow. The returned value is an optimal lower bound.*/
void Compute4MomentShadowIntensity(out float OutShadowIntensity, float4 _4Moments, float FragmentDepth, float DepthBias, float MomentBias)
{
    // Bias input data to avoid artifacts
    float4 b = lerp(_4Moments, float4(0.5f, 0.5f, 0.5f, 0.5f), MomentBias);
    float3 z;
    z[0] = FragmentDepth - DepthBias;

    // Compute a Cholesky factorization of the Hankel matrix B storing only non-
    // trivial entries or related products
    float L32D22 = mad(-b[0], b[1], b[2]);
    float D22 = mad(-b[0], b[0], b[1]);
    float SquaredDepthVariance = mad(-b[1], b[1], b[3]);
    float D33D22 = dot(float2(SquaredDepthVariance, -L32D22), float2(D22, L32D22));
    float InvD22 = 1.0f / D22;
    float L32 = L32D22*InvD22;

    // Obtain a scaled inverse image of bz=(1,z[0],z[0]*z[0])^T
    float3 c = float3(1.0f, z[0], z[0] * z[0]);
    // Forward substitution to solve L*c1=bz
    c[1] -= b.x;
    c[2] -= b.y + L32*c[1];
    // Scaling to solve D*c2=c1
    c[1] *= InvD22;
    c[2] *= D22 / D33D22;
    // Backward substitution to solve L^T*c3=c2
    c[1] -= L32*c[2];
    c[0] -= dot(c.yz, b.xy);
    // Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions z[1] 
    // and z[2]
    float p = c[1] / c[2];
    float q = c[0] / c[2];
    float D = ((p*p) / 4.0f) - q;
    float r = sqrt(D);
    z[1] = -(p / 2.0f) - r;
    z[2] = -(p / 2.0f) + r;

    // Use a solution made of four deltas if the solution with three deltas is invalid
    if (z[1]<0.0f || z[2]>1.0f) {
        float zFree = ((b[2] - b[1])*z[0] + b[2] - b[3]) / ((b[1] - b[0])*z[0] + b[1] - b[2]);
        float w1Factor = (z[0]>zFree) ? 1.0f : 0.0f;
        OutShadowIntensity = (b[1] - b[0] + (b[2] - b[0] - (zFree + 1.0f)*(b[1] - b[0]))*(zFree - w1Factor - z[0])
            / (z[0] * (z[0] - zFree))) / (zFree - w1Factor) + 1.0f - b[0];
    }
    // Use the solution with three deltas
    else {
        float4 Switch =
            (z[2]<z[0]) ? float4(z[1], z[0], 1.0f, 1.0f) : (
            (z[1]<z[0]) ? float4(z[0], z[1], 0.0f, 1.0f) :
                float4(0.0f, 0.0f, 0.0f, 0.0f));
        float Quotient = (Switch[0] * z[2] - b[0] * (Switch[0] + z[2]) + b[1]) / ((z[2] - Switch[1])*(z[0] - z[1]));
        OutShadowIntensity = Switch[2] + Switch[3] * Quotient;
    }
    OutShadowIntensity = saturate(OutShadowIntensity);
}

/*! Given a sampled value from a four-moment shadow map and a computed shadow map
depth for a point at the same location this function outputs 1.0, if the fragment
is in shadow 0.0f, if the fragment is lit and an intermediate value for partial
shadow. The returned value is an optimal lower bound except for the fact that it
does not exploit the knowledge that the original distribution has support in
[0,1].*/
void Compute4MomentUnboundedShadowIntensity(out float OutShadowIntensity,
    float4 _4Moments, float FragmentDepth, float DepthBias, float MomentBias)
{
    // Bias input data to avoid artifacts
    float4 b = lerp(_4Moments, float4(0.5f, 0.5f, 0.5f, 0.5f), MomentBias);
    float3 z;
    z[0] = FragmentDepth - DepthBias;

    // Compute a Cholesky factorization of the Hankel matrix B storing only non-
    // trivial entries or related products
    float L32D22 = mad(-b[0], b[1], b[2]);
    float D22 = mad(-b[0], b[0], b[1]);
    float SquaredDepthVariance = mad(-b[1], b[1], b[3]);
    float D33D22 = dot(float2(SquaredDepthVariance, -L32D22), float2(D22, L32D22));
    float InvD22 = 1.0f / D22;
    float L32 = L32D22*InvD22;

    // Obtain a scaled inverse image of bz=(1,z[0],z[0]*z[0])^T
    float3 c = float3(1.0f, z[0], z[0] * z[0]);
    // Forward substitution to solve L*c1=bz
    c[1] -= b.x;
    c[2] -= b.y + L32*c[1];
    // Scaling to solve D*c2=c1
    c[1] *= InvD22;
    c[2] *= D22 / D33D22;
    // Backward substitution to solve L^T*c3=c2
    c[1] -= L32*c[2];
    c[0] -= dot(c.yz, b.xy);
    // Solve the quadratic equation c[0]+c[1]*z+c[2]*z^2 to obtain solutions 
    // z[1] and z[2]
    float p = c[1] / c[2];
    float q = c[0] / c[2];
    float D = (p*p*0.25f) - q;
    float r = sqrt(D);
    z[1] = -p*0.5f - r;
    z[2] = -p*0.5f + r;
    // Compute the shadow intensity by summing the appropriate weights
    float4 Switch =
        (z[2]<z[0]) ? float4(z[1], z[0], 1.0f, 1.0f) : (
        (z[1]<z[0]) ? float4(z[0], z[1], 0.0f, 1.0f) :
            float4(0.0f, 0.0f, 0.0f, 0.0f));
    float Quotient = (Switch[0] * z[2] - b[0] * (Switch[0] + z[2]) + b[1]) / ((z[2] - Switch[1])*(z[0] - z[1]));
    OutShadowIntensity = Switch[2] + Switch[3] * Quotient;
    OutShadowIntensity = saturate(OutShadowIntensity);
}

/*! This function takes a sample from the four-moment optimized shadow map in the
given sampler using the given texture coordinate converts to moments and outputs
it. The shadow map dimensions and reciproque dimensions are needed to correct
off-by-one errors.*/
float4 deoptimize_moments(float4 optimized_moments)
{
    float4 OptimizedSample = optimized_moments;
    OptimizedSample[0] -= 0.035955884801f;
    float4 Out4Moments = mul(OptimizedSample, float4x4(
        0.2227744146f, 0.1549679261f, 0.1451988946f, 0.163127443f,
        0.0771972861f, 0.1394629426f, 0.2120202157f, 0.2591432266f,
        0.7926986636f, 0.7963415838f, 0.7258694464f, 0.6539092497f,
        0.0319417555f, -0.1722823173f, -0.2758014811f, -0.3376131734f));

    return Out4Moments;
}

Texture2D<float4> g_shadow_moments         : register(t1);

[RootSignature( MyRS1 ) ]
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
    light_ps.xy                       = light_ps.xy * 0.5f + 0.5f;
    light_ps.y                        = 1.0f - light_ps.y;

    float4 optimized_moments4         = g_shadow_moments.Sample(g_linear_clamp, light_ps.xy).xyzw;
    float4 moments4                   = deoptimize_moments(optimized_moments4);
    float  fragment_depth             = 1.0f - light_ps.z;
    float  depth_bias                 = 0.00000f;
    float  moment_bias              = 0.00003f;

    float  shadow_intensity           = 0.0f;

    Compute4MomentShadowIntensity(shadow_intensity, moments4, fragment_depth, depth_bias, moment_bias);

    shadow_intensity = 1.0f - shadow_intensity;
    /*

    float  shadow                     = g_shadow_moments.Sample(g_linear_clamp,  light_ps.xy ).x;

    float  shadow_intensity           = shadow > light_ps.z ? 0.0f : 1.0f; 

    */

    float  ndotl                      = saturate(dot(normal_ws, sun_light_direction_ws));
    float3 ambient                    = float3(0.2f, 0.2f, 0.2f);
    
    //return float4 (shadow_intensity * ndotl * albedo * sun_light_intensity + ambient, 0.0);
    return float4 (shadow_intensity, shadow_intensity, shadow_intensity, 0.0);
}
