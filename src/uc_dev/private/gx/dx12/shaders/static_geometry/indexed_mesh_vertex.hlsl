#include "../default_signature.hlsli"

#include "frame.hlsli"
#include "transform.hlsli"

struct interpolants
{
    float4 position     : SV_POSITION0;
};

struct input
{
    float3 position : position;
};

cbuffer per_draw_call : register(b1)
{
    float4x4 m_world;
};

[RootSignature( MyRS1 ) ]
interpolants main(input i)
{
    interpolants r;
    point_os position_os = make_point_os(i.position);
    
    r.position = project_p_os(position_os, m_world, m_view, m_perspective).m_value;

    return r;
}





