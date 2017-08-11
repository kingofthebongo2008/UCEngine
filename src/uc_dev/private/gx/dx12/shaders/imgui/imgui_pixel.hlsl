#include "../default_signature.hlsli"

struct interpolants
{
    float4 position     : SV_POSITION0;
    float2 uv           : texcoord0;
    float4 color        : color0;
};

Texture2D<float4> t         : register(t1);
SamplerState g_linear       : register(s0)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};


[RootSignature( MyRS1 ) ]
float4 main( interpolants r ) : SV_Target0
{
    float4 c = float4(r.color.rgba);
    float2 te = float2(r.uv.x, r.uv.y);

    float4 textured     = t.Sample(g_linear, te).rgba;
    float4 result       = c * textured;
    result.rgb          = result.rgb * result.a;
    //result.a            = 1.0f;
    return result;
}





