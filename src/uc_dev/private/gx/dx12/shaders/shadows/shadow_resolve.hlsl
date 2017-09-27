#include "../default_signature.hlsli"

[numthreads( 64, 1, 1 )]
[RootSignature( MyRS1 ) ]
void main( uint3 dtid : SV_DispatchThreadID )
{

}
