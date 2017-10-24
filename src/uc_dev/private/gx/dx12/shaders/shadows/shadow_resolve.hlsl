#include "../default_signature.hlsli"


Texture2DMS<float, 4>  g_shadows_buffer:register(t1);
RWTexture2D<float4>    g_shadow_moments:register(u1);

float p2(float x)
{
    return x * x;
}

float p3(float x)
{
    return x * x * x;
}

float p4(float x)
{
    return x * x * x * x;
}

/*! Given a scalar this function outputs a vector consisting of the first, second,
third and fourth power of this scalar linearly transformed using an optimized
basis for optimal quantization.*/
float4 compute_moment_vector4_moments_optimized(float moment_0, float moment_1, float moment_2, float moment_3)
{
    const float4 moments        = float4(moment_0, moment_1, moment_2, moment_3);
    const float4x4 transform    = float4x4
        (
            -2.07224649f,   13.7948857237f,     0.105877704f,   9.7924062118f,
            32.23703778f,   -59.4683975703f,    -1.9077466311f, -33.7652110555f,
            -68.571074599f, 82.0359750338f,     9.3496555107f,  47.9456096605f,
            39.3703274134f, -35.364903257f,     -6.6543490743f, -23.9728048165f
        );

    float4 moments_optimized = mul(moments, transform);

    //add bias
    moments_optimized[0] += 0.035955884801f;

    return moments_optimized;
}

[numthreads( 16, 16, 1 )]
[RootSignature( MyRS2 ) ]
void main( uint3 dtid : SV_DispatchThreadID )
{
   const uint2 location = dtid.xy;

   float z0 = 1.0f - g_shadows_buffer.Load(location,0);
   float z1 = 1.0f - g_shadows_buffer.Load(location,1);
   float z2 = 1.0f - g_shadows_buffer.Load(location,2);
   float z3 = 1.0f - g_shadows_buffer.Load(location,3);

   float moment_0 = (z0 + z1 + z2 + z3) / 4;
   float moment_1 = (p2(z0) + p2(z1) + p2(z2) + p2(z3)) / 4;
   float moment_2 = (p3(z0) + p3(z1) + p3(z2) + p3(z3)) / 4;
   float moment_3 = (p4(z0) + p4(z1) + p4(z2) + p4(z3)) / 4;

   g_shadow_moments[location] = compute_moment_vector4_moments_optimized( moment_0, moment_1, moment_2, moment_3);
}
