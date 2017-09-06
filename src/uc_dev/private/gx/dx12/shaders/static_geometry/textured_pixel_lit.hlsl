#include "../default_signature.hlsli"

struct interpolants
{
    float4 position       : SV_POSITION0;
    float2 uv             : texcoord0;
    float3 position_ws    : position0;
};

float checker_board_pattern(float2 uv)
{
    float2 c = floor(uv) / 2;
    return frac(c.x + c.y) * 2;
}

float4 checker_board(float2 uv)
{
    float2 uv_scaled = uv * float2(32.0f, 32.0f);
    float checker = checker_board_pattern(uv_scaled);
    return float4(checker, checker, checker, 1.0f);
}

Texture2D<float4> t         : register(t1);
SamplerState g_linear       : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


[RootSignature( MyRS1 ) ]
float4 main( interpolants r ) : SV_Target0
{
    float3 dudx                       = ddx(r.position_ws);
    float3 dudy                       = ddy(r.position_ws);
    float3 normal_ws                  = normalize(cross(dudx, dudy));
    float3 sun_light_direction_ws     = normalize(float3(1.0, 0.5, 0.5));
    float3 sun_light_intensity        = float3(0.5, 0.5, 0.5);
    float3 albedo                     = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float  ndotl                      = saturate(dot(normal_ws, sun_light_direction_ws));
    float3 ambient                    = float3(0.2f, 0.2f, 0.2f);
    
    return float4 (ndotl * albedo * sun_light_intensity + ambient, 0.0);
}
