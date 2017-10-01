#include "../default_signature.hlsli"


Texture2DMS<float, 4>  g_shadows_buffer:register(t1);
RWTexture2D<float4>    g_shadow_moments:register(u1);

[numthreads( 16, 16, 1 )]
[RootSignature( MyRS1 ) ]
void main( uint3 dtid : SV_DispatchThreadID )
{
   float location = dtid.xy;

   float z0 = g_shadows_buffer.Load(location, 0, 0);
   float z1 = g_shadows_buffer.Load(location, 0, 1);
   float z2 = g_shadows_buffer.Load(location, 0, 2);
   float z3 = g_shadows_buffer.Load(location, 0, 3);

   g_shadow_moments[uint2(dtid.xy)] = (z0+z1+z2+z3)/4;
}
