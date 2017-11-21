#include "../default_signature.hlsli"
#include "../shadows.hlsli"
#include "moment_shadow_maps_utils.hlsli"
 

/*! Values below this threshold are clamped since this scarcely ever changes the
lower bounds significantly but saves space.
\sa WarpFourthMomentOffset()*/
static const float MinFourthMomentOffset = 1.2e-7f;
/*! Since we only consider moments resulting from depths between -1 and 1, the
fourth moment offset has a global upper bound.
\sa WarpFourthMomentOffset()*/
static const float MaxFourthMomentOffset = 0.25f;
/*! This value would map to minus infinity and is used for normalization.
\sa WarpFourthMomentOffset()*/
static const float FourthMomentOffsetSingularity = 1.0e-7f;



/*! This is a helper function for QuantizeMomentsNonLinear32Bit() and its variants.
With the outputs lerp(pow(OutDepth.x,j),pow(OutDepth.y,j),OutWeight) equals the
j-th biased moment for j in {0,1,2,3}. For the fourth moment
OutFourthMomentOffset has to be added.*/
void prepare_non_linear_moment_quantization(out float2 OutDepth, out float OutWeight, out float OutFourthMomentOffset, float4 UnbiasedMoments)
{
    // Apply very slight biasing right now
    float4 b = lerp(UnbiasedMoments, float4(0.0f, 0.375f, 0.0f, 0.375f), 3.0e-7f);

    // Apply a Vandermonde decomposition
    float3 Polynomial;
    float Variance = mad(-b.x, b.x, b.y);
    Polynomial.z = Variance;
    Polynomial.y = mad(b.x, b.y, -b.z);
    Polynomial.x = dot(-b.xy, Polynomial.yz);

    float InvVariance = 1.0f / Polynomial[2];
    float p = Polynomial[1] * InvVariance;
    float q = Polynomial[0] * InvVariance;
    float D = p*p*0.25f - q;
    float r = sqrt(D);

    OutDepth = float2(-0.5f*p - r, -0.5f*p + r);
    OutWeight = (b.x - OutDepth[0]) / (OutDepth[1] - OutDepth[0]);

    // Compute small parts of a Choleky factorization to get the offset of the fourth 
    // moment

    float L21D11 = mad(-b.x, b.y, b.z);
    float SquaredDepthVariance = mad(-b.y, b.y, b.w);
    OutFourthMomentOffset = mad(-L21D11, L21D11*InvVariance, SquaredDepthVariance);
}

/*! This function warps the fourth moment offset which is output by
PrepareNonLinearMomentQuantization() in a non-linear fashion that will make the
distribution far more uniform. Small values (below 1.2e-7) are clamped. Output
values are guaranteed to be in the range from zero to one.
\sa UnwarpFourthMomentOffset() */
float warp_fourth_moment_offset(float FourthMomentOffset)
{
    // Compute the coefficients for the linear transform mapping to [0,1] (these are 
    // compile-time constants)
    const float Factor = 1.0f / (log(log(MaxFourthMomentOffset / FourthMomentOffsetSingularity)) - log(log(MinFourthMomentOffset / FourthMomentOffsetSingularity)));
    const float Summand = -log(log(MinFourthMomentOffset / FourthMomentOffsetSingularity))*Factor;
    // Apply the warp
    return saturate(mad(log(log(max(MinFourthMomentOffset, FourthMomentOffset) / FourthMomentOffsetSingularity)), Factor, Summand));
}


/*! This is the inverse function of WarpFourthMomentOffset(). It fulfills
UnwarpFourthMomentOffset(WarpFourthMomentOffset(x))==x for x>=1.2e-7.
\sa WarpFourthMomentOffset() */
float unwarp_fourth_moment_offset(float WarpedFourthMomentOffset) {
    // In the end, a linear transform has to map the value to the range from zero to 
    // one
    const float Summand = log(log(MinFourthMomentOffset / FourthMomentOffsetSingularity));
    const float Factor = log(log(MaxFourthMomentOffset / FourthMomentOffsetSingularity)) - Summand;
    return exp(exp(mad(WarpedFourthMomentOffset, Factor, Summand)))*FourthMomentOffsetSingularity;
}

/*! Given a vector of moments from signed depths with single precision and no
biasing this function turns it into a compact representation using only 32 bits.
This is accomplished by a non-linear quantization transform.*/
uint quantize_moments_non_linear_32_bit(float4 unbiased_moments)
{
    float2 Depth;
    float Weight;
    float FourthMomentOffset;

    prepare_non_linear_moment_quantization(Depth, Weight, FourthMomentOffset, unbiased_moments);

    // The weight often maps to a shadow intensity directly, so quantizing the 
    // square root makes the banding perceptionally less problematic
    float WeightPrepared = sqrt(saturate(Weight));
    float FourthMomentOffsetPrepared = warp_fourth_moment_offset(FourthMomentOffset);
    uint WeightQuantized = min(uint(WeightPrepared*pow(2.0f, 7.0f)), pow(2, 7) - 1);
    uint FourthMomentOffsetQuantized = min(uint(FourthMomentOffsetPrepared*pow(2.0f, 6.0f)), pow(2, 6) - 1);

    // Quantize depth to 10 bits per value using only values from 0 to 1022 to 
    // enable exploitation of Depth[0]<Depth[1]
    uint2 DepthQuantized = uint2(mad(clamp(Depth, -1.0f, 0.999f), 511.5f, 511.5f));
    // Save one bit by exploiting Depth[0]<Depth[1]
    DepthQuantized = (DepthQuantized[0] >= 512) ? (uint2(1023, 1022) - DepthQuantized) : DepthQuantized;
    // Pack everything into a single integer and write the result
    return (DepthQuantized[0] << 23 | DepthQuantized[1] << 13 | WeightQuantized << 6 | FourthMomentOffsetQuantized);
}


/*! Takes the output of QuantizeMomentsNonLinear32Bit() and reconstructs the output
of PrepareNonLinearMomentQuantization() except for quantization errors.*/
void unpack_moments_non_linear_32_bit(out float2 OutDepth, out float OutWeight, out float OutFourthMomentOffset, uint PackedDistribution)
{
    uint2 DepthQuantized    = (PackedDistribution&uint2(0xFF800000, 0x007FE000)) >> uint2(23, 13);
    DepthQuantized          = (DepthQuantized[0]>DepthQuantized[1]) ? (uint2(1023, 1022) - DepthQuantized) : DepthQuantized;
    OutDepth                = mad(float2(DepthQuantized), 1.0f / 511.5f, -1.0f + 2.0f / 1023.0f);
    OutWeight               = mad(float(PackedDistribution & 0x00001FC0), pow(0.5f, 6.0f + 7.0f), pow(0.5f, 7.0f + 1.0f));
    OutWeight               *= OutWeight;
    OutFourthMomentOffset   = unwarp_fourth_moment_offset(mad(float(PackedDistribution & 0x0000003F), pow(0.5f, 6.0f), pow(0.5f, 6.0f)));
}

void apply_resolve_32_bit(inout RWTexture2D<uint> shadow_moments, Texture2DMS<float, 4> shadows_buffer, uint3 dtid : SV_DispatchThreadID)
{
    const uint2 location = dtid.xy;

    float z0 = 1.0f - shadows_buffer.Load(location, 0);
    float z1 = 1.0f - shadows_buffer.Load(location, 1);
    float z2 = 1.0f - shadows_buffer.Load(location, 2);
    float z3 = 1.0f - shadows_buffer.Load(location, 3);

    float moment_0 = (z0 + z1 + z2 + z3) / 4;
    float moment_1 = (p2(z0) + p2(z1) + p2(z2) + p2(z3)) / 4;
    float moment_2 = (p3(z0) + p3(z1) + p3(z2) + p3(z3)) / 4;
    float moment_3 = (p4(z0) + p4(z1) + p4(z2) + p4(z3)) / 4;

    float4 unbiased_moments = float4(moment_0, moment_1, moment_2, moment_3);

    shadow_moments[location] = quantize_moments_non_linear_32_bit(unbiased_moments);
}
