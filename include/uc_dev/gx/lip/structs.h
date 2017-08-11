#pragma once

#include <uc_dev/lip/lip.h>

namespace uc
{
    namespace lip
    {
        struct float2
        {
            float m_x;
            float m_y;

            LIP_DECLARE_RTTI()
        };

        LIP_DECLARE_TYPE_ID(uc::lip::float2)

        struct float3
        {
            float m_x;
            float m_y;
            float m_z;

            LIP_DECLARE_RTTI()
        };

        LIP_DECLARE_TYPE_ID(uc::lip::float3)

        struct float4
        {
            float m_x;
            float m_y;
            float m_z;
            float m_w;

            LIP_DECLARE_RTTI()
        };

        LIP_DECLARE_TYPE_ID(uc::lip::float4)

		struct alignas(16) float4a
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;

			LIP_DECLARE_RTTI()
		};

		LIP_DECLARE_TYPE_ID(uc::lip::float4a)

        struct ubyte4
        {
            uint8_t m_x;
            uint8_t m_y;
            uint8_t m_z;
            uint8_t m_w;
            LIP_DECLARE_RTTI()
        };

        LIP_DECLARE_TYPE_ID(uc::lip::ubyte4)

        struct uint4
        {
            uint32_t m_x;
            uint32_t m_y;
            uint32_t m_z;
            uint32_t m_w;
            LIP_DECLARE_RTTI()
        };

        LIP_DECLARE_TYPE_ID(uc::lip::uint4)
    }
}
