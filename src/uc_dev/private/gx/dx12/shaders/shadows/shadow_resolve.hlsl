#include "../default_signature.hlsli"
#include "../shadows.hlsli"


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
