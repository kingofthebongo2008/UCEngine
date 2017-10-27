#include "../default_signature.hlsli"
#include "../shadows.hlsli"

#include "moment_shadow_maps.hlsli"

Texture2DMS<float, 4>        g_shadows_buffer:register(t1);
RWTexture2D<unorm float4>    g_shadow_moments:register(u1);

[numthreads( 32, 8, 1 )]
[RootSignature( MyRS2 ) ]
void main( uint3 dtid : SV_DispatchThreadID, uint3 GroupThreadID : SV_GroupThreadID, uint3 GroupID : SV_GroupID)
{
   ApplyResolveAndGaussian8_64Bit(g_shadow_moments, g_shadows_buffer, GroupThreadID, GroupID);
}
