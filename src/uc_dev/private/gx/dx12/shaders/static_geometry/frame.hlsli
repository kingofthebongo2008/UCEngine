#ifndef __per_frame_hlsli__
#define __per_frame_hlsli__
cbuffer per_frame   : register(b0)
{
    float4x4 m_view;
    float4x4 m_perspective;
};
#endif
