#ifndef __pbr_brdf_hlsli__
#ifndef __pbr_brdf_hlsli__

#include "pbr_common.hlsli"

// Microfacet specular = D*G*F / (4*NoL*NoV) = D*Vis*F
// Vis = G / (4*NoL*NoV)

float square(float x)
{
    return x * x;
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float d_ggx( float roughness, float ndoth )
{
    float a     = square(roughness);
    float a2    = a * a;
    float d     = ( ndoth * a2 - ndoth ) * ndoth + 1;
    return a2 / ( g_pi * d * d );                 
}

// Smith term for GGX
// [Smith 1967, "Geometrical shadowing of a random rough surface"]
float v_smith_ggx( float roughness, float ndotv, float ndotl )
{
    float a = square( roughness);
    float a2 = a*a;

    float smith_v = ndotv + sqrt( ndotv * (ndotv - ndotv * a2) + a2 );
    float smith_l = ndotl + sqrt( ndotl * (ndotl - ndotl * a2) + a2 );
    return rcp( smith_v + smith_l);
}

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

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float v_smith_joint_approximate( float roughness, float ndotv, float ndotl )
{

    float a         = square( roughness );
    float smith_v   = ndotl * ( ndotv * ( 1 - a ) + a );
    float simith_l  = ndotv * ( ndotl * ( 1 - a ) + a );

    // Note: will generate NaNs with roughness = 0.  min_roughness is used to prevent this
    return 0.5 * rcp( smith_v + smith_l );
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float3 f_schlick( float3 f0, float vdoth )
{
    float fc    = pow_5( 1 - vdoth );

    // Anything less than 2% is physically impossible and is instead considered to be shadowing
    float f90   = saturate (50.0 * dot ( f0, 0.33) );

    return lerp( f0, f90, fc );
   
}








#endif

