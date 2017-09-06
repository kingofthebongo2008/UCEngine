#ifndef __transform_hlsli__
#define __transform_hlsli__

struct point_ws
{
    float4 m_value;
};

struct point_ps
{
    float4 m_value;
};

struct point_vs
{
    float4 m_value;
};

struct point_os
{
    float4 m_value;
};

point_ws make_point_ws(float3 v)
{
    point_ws r;
    r.m_value = float4(v, 1.0f);
    return r;
}

point_vs make_point_vs(float3 v)
{
    point_ws r;
    r.m_value = float4(v, 1.0f);
    return r;
}

point_ps make_point_ps(float4 v)
{
    point_ps r;
    r.m_value = float4(v);
    return r;
}

point_os make_point_os(float3 v)
{
    point_ws r;
    r.m_value = float4(v, 1.0f);
    return r;
}

point_ps project_vertex(point_os v_os, float4x4 world, float4x4 view, float4x4 perspective)
{
    //three muls for greater accuracy
    float4 result = mul(mul(mul(v_os.m_value, world), view), perspective);

    return make_point_ps(result);
}

point_ws transform_world(point_os v_os, float4x4 world)
{
    float4 result = mul(v_os.m_value, world);
    return make_point_ws(result.xyz);
}

float4 project_vertex(float4 v_os, float4x4 world, float4x4 view, float4x4 perspective)
{
    //three muls for greater accuracy
    float4 result = mul( mul( mul( v_os, world ), view), perspective);
    return result;
}

#endif
