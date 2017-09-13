#ifndef __transform_hlsli__
#define __transform_hlsli__

struct point_ws
{
    float3 m_value;
};

struct point_ps
{
    float4 m_value;
};

struct point_vs
{
    float3 m_value;
};

struct point_os
{
    float3 m_value;
};

point_ws make_point_ws(float3 v)
{
    point_ws r;
    r.m_value = v;
    return r;
}

point_vs make_point_vs(float3 v)
{
    point_ws r;
    r.m_value = v;
    return r;
}

point_ps make_point_ps(float4 v)
{
    point_ps r;
    r.m_value = v;
    return r;
}

point_os make_point_os(float3 v)
{
    point_ws r;
    r.m_value = v;
    return r;
}

/////////////////////////////////////////////////////////////////
struct vector_ws
{
    float3 m_value;
};

struct vector_ps
{
    float4 m_value;
};

struct vector_vs
{
    float3 m_value;
};

struct vector_os
{
    float3 m_value;
};

vector_ws make_vector_ws(float3 v)
{
    vector_ws r;
    r.m_value = v;
    return r;
}

vector_vs make_vector_vs(float3 v)
{
    vector_ws r;
    r.m_value = v;
    return r;
}

vector_ps make_vector_ps(float4 v)
{
    vector_ps r;
    r.m_value = v;
    return r;
}

vector_os make_vector_os(float3 v)
{
    vector_ws r;
    r.m_value = v;
    return r;
}

/////////////////////////////////////////////////////////////////
struct euclidean_transform_3d
{
    float4x4 m_value;
};
struct affine_transform_3d
{
    float4x4 m_value;
};
struct projective_transform_3d
{
    float4x4 m_value;
};
/////////////////////////////////////////////////////////////////

point_ps project_p_os(point_os v_os, euclidean_transform_3d world, euclidean_transform_3d view, projective_transform_3d perspective)
{
    //three muls for greater accuracy
    float4 result = mul(mul(mul(float4(v_os.m_value, 1.0f), world.m_value), view.m_value), perspective.m_value);
    return make_point_ps(result);
}

point_ws transform_p_os(point_os v_os, euclidean_transform_3d world)
{
    float4 result = mul(float4(v_os.m_value, 1.0f), world.m_value);
    return make_point_ws(result.xyz);
}

vector_ws transform_v_os(vector_os v_os, euclidean_transform_3d world)
{
    float4 result = mul(float4(v_os.m_value, 0.0f), world.m_value);
    return make_point_ws(result.xyz);
}


#endif
