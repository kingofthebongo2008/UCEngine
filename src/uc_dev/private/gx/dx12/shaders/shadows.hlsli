#ifndef __shadows_hlsli__
#define __shadows_hlsli__

#include "shadows/moment_shadow_maps_compute.hlsli"
#include "shadows/moment_shadow_maps_32.hlsli"
#include "static_geometry/vector_space.hlsli"

struct sampler_moments_64
{
    Texture2D<float4>  m_shadow_moments_buffer;
    SamplerState	   m_sampler;
};

struct sampler_non_linear_moments_64
{
    Texture2D<uint4>        m_shadow_moments_buffer;
    Texture2DArray<uint2>   m_blue_noise;
};

struct sampler_non_linear_moments_32
{
    Texture2D<uint>         m_shadow_moments_buffer;
    Texture2DArray<uint2>   m_blue_noise;
};

struct view_transforms
{
    euclidean_transform_3d   m_view;
    projective_transform_3d  m_perspective;

    euclidean_transform_3d view()           { return m_view; }
    projective_transform_3d perspective()   { return m_perspective; }
};

struct shadow_view_transforms : view_transforms
{

};

struct main_view_transforms : view_transforms
{

};

struct moments_64_context
{
    shadow_view_transforms  m_shadow_transforms;

    euclidean_transform_3d  shadow_view()        { return m_shadow_transforms.view();}
    projective_transform_3d shadow_perspective() { return m_shadow_transforms.perspective();}
};

struct non_linear_moments_context
{
    shadow_view_transforms  m_shadow_transforms;
    main_view_transforms    m_main_view_transforms;

    uint3                   m_randomness;
    uint2                   m_shadow_buffer_size;
    uint2                   m_view_port_pixel_index;

    uint2                   blue_noise_xy() { return m_randomness.xy; }
    uint                    blue_noise_z() { return m_randomness.z; }

    uint2                   shadow_buffer_size() { return m_shadow_buffer_size; }

    euclidean_transform_3d  shadow_view() { return m_shadow_transforms.view(); }
    projective_transform_3d shadow_perspective() { return m_shadow_transforms.perspective(); }

    euclidean_transform_3d  main_view() { return m_main_view_transforms.view(); }
    projective_transform_3d main_perspective() { return m_main_view_transforms.perspective(); }

};

struct non_linear_moments_64_context : non_linear_moments_context
{
};

struct non_linear_moments_32_context : non_linear_moments_context
{
};

sampler_moments_64 make_sampler_moments_64( Texture2D<float4> t, SamplerState s )
{
   sampler_moments_64 r = { t, s };
   return r;
}

sampler_non_linear_moments_64 make_sampler_non_linear_moments_64(Texture2D<uint4> s, Texture2DArray<float2> noise)
{
    sampler_non_linear_moments_64 r = { s, noise };
    return r;
}

sampler_non_linear_moments_32 make_sampler_non_linear_moments_32(Texture2D<uint> s, Texture2DArray<uint2> noise)
{
    sampler_non_linear_moments_32 r = { s, noise };
    return r;
}

shadow_view_transforms make_shadow_transforms( euclidean_transform_3d v, projective_transform_3d p )
{
    shadow_view_transforms r = { v, p };
    return r;
}

moments_64_context make_moments_64_context(shadow_view_transforms t)
{
    moments_64_context r = {t};
    return r;
}

struct shadow_fragment_depth
{
    float2 m_shadow_buffer_uv;
    float  m_fragment_depth;

    float  fragment_depth() { return m_fragment_depth; }
    float2 shadow_buffer_uv() { return m_shadow_buffer_uv; }
};

shadow_fragment_depth compute_fragment_depth(float3 position_ws, euclidean_transform_3d shadow_view, projective_transform_3d shadow_perspective)
{
    float4 light_ps         = project_p_ws(make_point_ws(position_ws), shadow_view, shadow_perspective).m_value;
    light_ps                = light_ps / light_ps.w;
    float2 shadow_map_uv    = mad(light_ps.xy, float2(0.5f, -0.5f), float2(0.5f, 0.5f));
    float  fragment_depth   = 1.0f - light_ps.z; //shadows are stored in 1-0, not 0-1
    shadow_fragment_depth r = { shadow_map_uv, fragment_depth };
    return r;
}

float compute_moment4_shadow_maps(sampler_moments_64 moments, float3 position_ws, moments_64_context ctx )
{
    shadow_fragment_depth s           = compute_fragment_depth(position_ws, ctx.shadow_view(), ctx.shadow_perspective());
    float4 optimized_moments4         = moments.m_shadow_moments_buffer.Sample( moments.m_sampler, s.shadow_buffer_uv() ) .xyzw;
    float4 moments4                   = compute_deoptimized_moments(optimized_moments4);

    float  shadow_intensity           = compute4_moment_shadow_intensity( moments4, s.fragment_depth());
    shadow_intensity                  = 1.0f - shadow_intensity;
    return shadow_intensity;
}

void ComputeViewportPixelIndex(out int2 OutViewportPixelIndex, float4 ProjectionSpacePosition, float4 ViewportTransform )
{
    ProjectionSpacePosition.xy /= ProjectionSpacePosition.w;
    OutViewportPixelIndex = int2(mad(ProjectionSpacePosition.xy, ViewportTransform.xy, ViewportTransform.zw));
}

uint2 compute_view_port_pixel_index( float3 position_ws )
{
    euclidean_transform_3d  main_view;
}

float compute_nonlinear_moment4_shadow_maps_64(sampler_non_linear_moments_64 moments, float3 position_ws, non_linear_moments_64_context ctx)
{
    shadow_fragment_depth s         = compute_fragment_depth(position_ws, ctx.shadow_view(), ctx.shadow_perspective());

    uint2  view_port_pixel_index    = ctx.m_view_port_pixel_index;
    float2 blue_noise               = moments.m_blue_noise.Load(uint4( ( view_port_pixel_index.xy + ctx.blue_noise_xy() ) & 0x3F, ctx.blue_noise_z() & 0x3F, 0));
    float4 optimized_moments4       = moments.m_shadow_moments_buffer.Load( uint3( s.shadow_buffer_uv() * ctx.shadow_buffer_size() + blue_noise.xy - 0.5f, 0 ) );
    float4 moments4                 = compute_deoptimized_moments(optimized_moments4);

    float  shadow_intensity         = compute4_moment_shadow_intensity(moments4, s.fragment_depth());
    shadow_intensity                = 1.0f - shadow_intensity;

    return shadow_intensity;
}

/*! This function computes a lower bound to the cumulative distribution function on 
   the reals from four non-linearly quantized moments. The moments of order j=0 to 
   j=3 are given by lerp(pow(Support[0],j),pow(Support[1],j),Weight). The fourth 
   moment is given by the same formula except that FourthMomentOffset has to be 
   added. The lower bound is computed for the interval (-infinity,IntervalEnd].*/
float compute_moment4_non_linear_lower_bound(float IntervalEnd,float2 Support,float Weight,float FourthMomentOffset)
{
	// Avoid zero variance
	const float ClampingOffset=5.0e-6f;
	Weight=clamp(Weight,ClampingOffset,1.0f-ClampingOffset);
	Support.y=max(Support.x+ClampingOffset,Support.y);
	// Is the fragment fully lit?
	[branch] if(IntervalEnd<=Support.x){
		return 0.0f;
	}
	// Normalize the interval end such that Support can be treated as 
	// float2(0.0f,1.0f). The fourth moment offset needs to be scaled accordingly.
	float Scaling=rcp(Support[1]-Support[0]);
	float NormalizedIntervalEnd=(IntervalEnd-Support[0])*Scaling;
	float ScalingSquared=Scaling*Scaling;
	FourthMomentOffset*=ScalingSquared*ScalingSquared;
	// Prepare a few quantities that will be needed repeatedly
	float InvWeight0=rcp(1.0f-Weight);
	float InvWeight1=rcp(Weight);
	float InvFourthMomentOffset=rcp(FourthMomentOffset);
	// Is the most complicated case present?
	float Root=NormalizedIntervalEnd;
	float FlippedRoot=1.0f-Root;
	float2 OutputTransform=float2(-1.0f,1.0f);
	[branch] if(NormalizedIntervalEnd<1.0f){
		float q=-FourthMomentOffset*InvWeight0/NormalizedIntervalEnd;
		float pHalf=mad(-0.5f*q,mad(NormalizedIntervalEnd,-InvWeight1,1.0f)/FlippedRoot,-0.5f);
		Root=-pHalf-sqrt(mad(pHalf,pHalf,-q));
		FlippedRoot=1.0f-Root;
		OutputTransform=float2(1.0f,0.0f);
	}
	// Compute the weight
	float RootSquared=Root*Root;
	float FlippedRootSquared=FlippedRoot*FlippedRoot;
	float RootWeight=1.0f/dot(
		float3(InvWeight0,        InvWeight1, InvFourthMomentOffset),
		float3(FlippedRootSquared,RootSquared,RootSquared*FlippedRootSquared));
	return mad(RootWeight,OutputTransform.x,OutputTransform.y);
}


/*! This function warps the fourth moment offset which is output by
PrepareNonLinearMomentQuantization() in a non-linear fashion that will make the
distribution far more uniform. Small values (below 1.2e-7) are clamped. Output
values are guaranteed to be in the range from zero to one.
\sa UnwarpFourthMomentOffset() */
float WarpFourthMomentOffset(float FourthMomentOffset) {
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
float UnwarpFourthMomentOffset(float WarpedFourthMomentOffset) {
    // In the end, a linear transform has to map the value to the range from zero to 
    // one
    const float Summand = log(log(MinFourthMomentOffset / FourthMomentOffsetSingularity));
    const float Factor = log(log(MaxFourthMomentOffset / FourthMomentOffsetSingularity)) - Summand;
    return exp(exp(mad(WarpedFourthMomentOffset, Factor, Summand)))*FourthMomentOffsetSingularity;
}


/*! Takes the output of QuantizeMomentsNonLinear32Bit() and reconstructs the output 
   of PrepareNonLinearMomentQuantization() except for quantization errors.*/
void UnpackMomentsNonLinear32Bit(out float2 OutDepth,out float OutWeight,out float OutFourthMomentOffset,uint PackedDistribution){
	uint2 DepthQuantized=(PackedDistribution&uint2(0xFF800000,0x007FE000))>>uint2(23,13);
	DepthQuantized=(DepthQuantized[0]>DepthQuantized[1])?(uint2(1023,1022)-DepthQuantized):DepthQuantized;
	OutDepth=mad(float2(DepthQuantized),1.0f/511.5f,-1.0f+2.0f/1023.0f);
	OutWeight=mad(float(PackedDistribution&0x00001FC0),pow(0.5f,6.0f+7.0f),pow(0.5f,7.0f+1.0f));
	OutWeight*=OutWeight;
	OutFourthMomentOffset=UnwarpFourthMomentOffset(mad(float(PackedDistribution&0x0000003F),pow(0.5f,6.0f),pow(0.5f,6.0f)));
}

float compute_nonlinear_moment4_shadow_maps_32(sampler_non_linear_moments_32 moments, float3 position_ws, non_linear_moments_32_context ctx)
{
    shadow_fragment_depth s         = compute_fragment_depth(position_ws, ctx.shadow_view(), ctx.shadow_perspective());
    uint2  view_port_pixel_index    = ctx.m_view_port_pixel_index;

    float2  blue_noise = moments.m_blue_noise.Load(uint4((view_port_pixel_index.xy + ctx.blue_noise_xy()) & 0x3F, ctx.blue_noise_z() & 0x3F, 0)) / float2(63.0f, 63.0f);

    //blue_noise = float2(0.5f, 0.5f);
    uint   packed_distribution      = moments.m_shadow_moments_buffer.Load(uint3(s.shadow_buffer_uv() * ctx.shadow_buffer_size() + blue_noise.xy - 0.5f, 0));

    float2 out_depth;
    float  out_weight;
    float  out_fourth_moment;

    unpack_moments_non_linear_32_bit(out_depth, out_weight, out_fourth_moment, packed_distribution);

    float  depth_bias       = 0.0010f;
    float  biased_depth     = s.fragment_depth() - depth_bias;
    float  shadow_intensity = compute_moment4_non_linear_lower_bound(biased_depth, out_depth,out_weight, out_fourth_moment);
    shadow_intensity        = 1.0f - shadow_intensity;

    return shadow_intensity;
}

    


#endif
