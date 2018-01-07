#ifndef __pbr_reference_hlsli__
#define __pbr_reference_hlsli__

//Moving frostbite engine to pbr 
//Listing A.1 shows a simple kernel for integrating a light probe with a specular GGX microfacet model in order to quickly see the expected results.

//paper Building an Orthonormal Basis, Revisited
float copysignf( float x, float y )
{
    return abs(x) * sgn ( y );
}

void branchless_onb( in float3 n, out float3 b1, out float3 b2)
{
    const float sign = copysignf(1.0f, n.z);
    const float a = -1.0f / (sign + n.z);
    const float b = n.x * n.y * a;
    b1 = float3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
    b2 = float3(b, sign + n.y * n.y * a, -n.y);
}

static float2 get_sample ( uint32_t sample, uint32_t sample_count )
{
    //todo:
    return 0.0f;
}

static const uint32_t g_sample_count = 1024;
static const float    g_pi = 3.1415f;

float3 pbr_reference_compute_specular_ibl_reference( in float3 n, in float3 v, in float roughness, in float3 f0, in float3 f90 )
{
    //build local referential
    //float3 up_vector = abs(n.z) < 0.999 ? float3(0, 0, 1) : float3 ( 1, 0, 0 );
    //float3 tangent_x = normalize( cross ( up_vector, n ) );
    //float3 tangent_y = cross ( n, tangent_x );

    float3 tangent_x;
    float3 tangent_y;

    branchless_onb( n, tangent_x, tangent_y);

    float3 accumulated_light = 0;
    for ( uint32_t i = 0; i < g_sample_count; ++i )
    {
        float2 u = get_sample( i, g_sample_count );

        //ggx ndf sampling
        float cos_theta_h = sqrt( 1-u.x ) / ( 1 + (roughness * roughness - 1) * u.x );
        float sin_theta_h = sqrt( 1- min( 1, cos_theta_h, cos_theta_h ) );
        float phi_h       = u.y * g_pi / 2;

        
 
    }


}


#endif
