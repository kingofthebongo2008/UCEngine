#ifndef __pbr_brdf_hlsli__
#ifndef __pbr_brdf_hlsli__

#include "pbr_common.hlsli"


//identities
// | L + V | ^ 2 = 2 + 2L.V
// 0.5 + 0.5 L.V = 1 / 4 |L + V|^2
// N.H = ( N.L + N.V ) / | L + V |
// L.H = V.H = 1 / 2 |L + V|

void ndoth_vdoth( float ldotv, float ndotl, float ndotv, out float ndoth, out float ldoth )
{
    float lv     = 2 + 2 * ldotv;
    float rcp_lv = rqsrt( lv );

    ndoth = (ndotl + ndotv) * rcp_lv;
    ldoth = rcp_lv + rcp_lv * ldotv;
}

float ndoth( float ldotv, float ndotl, float ndotv)
{
    float lv     = 2 + 2 * ldotv;
    float rcp_lv = rqsrt( lv );

    float ndoth = (ndotl + ndotv) * rcp_lv;
    return ndoth;
}

float ldoth( float ldotv, float ndotl, float ndotv)
{
    float lv     = 2 + 2 * ldotv;
    float rcp_lv = rqsrt( lv );

    float ldoth  = rcp_lv + rcp_lv * ldotv;
    return ldoth;
}

float pow_2(float x)
{
    return x * x;
}

float pow_4(float x)
{
    return pow_2(x) * pow_2(x);
}

float pow_5(float x)
{
    return pow_2(x) * pow_2(x) * x;
}

float d_ggx( float alpha_g4, float ndoth )
{
    float a2    = alpha_g4;
    float d     = ( ndoth * a2 - ndoth ) * ndoth + 1;
    return a2 / (  d * d );                 
}

float v_smith_ggx_correlated( float alpha_g4, float ndotv, float ndotl )
{
    float a2            = alpha_g4;

    // Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
    float smith_ggx_v   = ndotl * sqrt (( -ndotv * a2 + ndotv ) * ndotv + a2 );
    float smith_ggx_l   = ndotv * sqrt (( -ndotl * a2 + ndotl ) * ndotl + a2 );
    return 0.5f * rcp ( smith_ggx_v + smith_ggx_l );
}

float3 f_schlick( float3 f0, float vdoth )
{
    float fc    = pow_5( 1 - vdoth );

    // Anything less than 2% is physically impossible and is instead considered to be shadowing
    float f90   = saturate (50.0 * dot ( f0, 0.33) );
    return lerp( f0, f90, fc );
}


// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
/*
float v_smith_joint_approximate( float roughness, float ndotv, float ndotl )
{
    float a         = pow_2( roughness );
    float smith_v   = ndotl * ( ndotv * ( 1 - a ) + a );
    float simith_l  = ndotv * ( ndotl * ( 1 - a ) + a );

    // Note: will generate NaNs with roughness = 0.  min_roughness is used to prevent this
    return 0.5 * rcp( smith_v + smith_l );
}
*/
/*
half3 SpecularBRDF(half3 N, half3 V, half3 L, half m, half3 f0, half NormalizationFactor)
{
    half m2 = m * m;
    half3 H = normalize( V + L );

    // GGX NDF
    half NdotH = saturate( dot( N, H ) );
    half spec = (NdotH * m2 - NdotH) * NdotH + 1;
    spec = m2 / (spec * spec) * NormalizationFactor;
    
    // Correlated Smith Visibility Term (including Cook-Torrance denominator)
    half NdotL = saturate( dot( N, L ) );
    half NdotV = abs( dot( N, V ) ) + 1e-5h;
    half Gv = NdotL * sqrt( (-NdotV * m2 + NdotV) * NdotV + m2 );
    half Gl = NdotV * sqrt( (-NdotL * m2 + NdotL) * NdotL + m2 );
    spec *= 0.5h / (Gv + Gl);
        
    // Fresnel (Schlick approximation)
    half f90 = saturate( dot( f0, 0.33333h ) / 0.02h );  // Assume micro-occlusion when reflectance is below 2%
    half3 fresnel = lerp( f0, f90, pow( 1 - saturate( dot( L, H ) ), 5 ) );

    return fresnel * spec;
}
*/









#endif

