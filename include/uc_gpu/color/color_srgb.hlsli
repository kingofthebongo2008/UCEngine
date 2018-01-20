#ifndef __color_srgb_hlsli__
#ifndef __color_srgb_hlsli__

//reference functions, source wikipedia
float3 color_gamma_srgb( float3 v_linear_srgb )
{
    float3 v = v_linear_srgb;

    float  threshold        = 0.0031308;
    float  a                = 0.055;
    float3 a0               = 12.92 * v;
    float3 a1               = (1.0 + a) * pow( v, 1.0 / 2.4 ) - a;
    return v <= threshold ? a0 : a1;
}

float3 color_degamma_srgb( float3 v_gamma_srgb )
{
    float3 v = v_gamma_srgb;

    float  threshold = 0.04045;
    float  a         = 0.055;    
    float3 a0 = v / 12.92;
    float3 a1 = pow( ( (v + a) / (1 + a) ) , 2.4);

    return v <= threshold ? a0 : a1;
}

#endif
