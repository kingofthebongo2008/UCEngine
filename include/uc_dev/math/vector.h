#pragma once

#include <intrin.h>
#include <cstdint>

#include <uc_dev/math/defines.h>

namespace uc
{
    namespace math
    {
        typedef __m128 float4;

//1st 3 parameters, pass by value
#if defined(_X86) || defined(_X64) || defined(_ARM) || defined(_ARM64)
        using afloat4 = const float4;
#else
        using afloat4 = const float4&;
#endif

// Fix-up for (4th) XMVECTOR parameter to pass in-register for ARM, ARM64, and x64 vector call; by reference otherwise
#if defined(_ARM) || defined(_ARM64) || defined(_X64)
        using bfloat4 = const float4;
#else
        using bfloat4 = const float4&;
#endif

// Fix-up for (5th & 6th) XMVECTOR parameter to pass in-register for ARM64 and vector call; by reference otherwise
#if ( defined(_ARM)  ) 
        using cfloat4 = const float4;
#else
        using cfloat4 = const float4&;
#endif

        // Fix-up for (7th+) XMVECTOR parameters to pass by reference
        using dfloat4 = const float4&;

        enum component
        {
            x = 0,
            y = 1,
            z = 2,
            w = 3
        };

        //memory control and initialization

        inline float4 UC_MATH_CALL zero()
        {
            return _mm_setzero_ps();
        }

        inline float4 UC_MATH_CALL one()
        {
            return _mm_set_ps(1.0f, 1.0f, 1.0f, 1.0f);
        }

        inline float4 UC_MATH_CALL minus_one()
        {
            return _mm_set_ps(-1.0f, -1.0f, -1.0f, -1.0f);
        }

        inline float4 UC_MATH_CALL identity_r0()
        {
            static const float4 r0 = { 1.0f, 0.0f, 0.0f, 0.0f };
            return r0;
        }

        inline float4 UC_MATH_CALL identity_r1()
        {
            static const float4 r1 = { 0.0f, 1.0f, 0.0f, 0.0f };
            return r1;
        }

        inline float4 UC_MATH_CALL identity_r2()
        {
            static const float4 r2 = { 0.0f, 0.0f, 1.0f, 0.0f };
            return r2;
        }

        inline float4 UC_MATH_CALL identity_r3()
        {
            static const float4 r3 = { 0.0f, 0.0f, 0.0f, 1.0f };
            return r3;
        }

        template <uint32_t shuffle_mask> inline float4 UC_MATH_CALL shuffle(afloat4 value1, afloat4 value2)
        {
            return _mm_shuffle_ps(value1, value2, shuffle_mask);
        }

        //until we get constexpr in visual studio 2012
        #define shuffle_mask(v1, v2, v3, v4) _MM_SHUFFLE(v4, v3, v2, v1)

        /*
        template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline uint32_t shuffle_mask()
        {
            return _MM_SHUFFLE(v4, v3, v2, v1);
        }
        */

        template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline float4 UC_MATH_CALL shuffle(afloat4 value1, afloat4 value2)
        {
            const uint32_t shuffle_k = _MM_SHUFFLE(v4, v3, v2, v1);
            return _mm_shuffle_ps(value1, value2, shuffle_k);
        }

        template <uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4> inline float4 UC_MATH_CALL swizzle(afloat4 value)
        {
            return shuffle<v1, v2, v3, v4>(value, value);
        }

        inline float4 UC_MATH_CALL merge_xy(afloat4 v1, afloat4 v2)
        {
            return _mm_unpacklo_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL merge_zw(afloat4 v1, afloat4 v2)
        {
            return _mm_unpackhi_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL splat(float value)
        {
            return _mm_set1_ps(value);
        }

        inline float4 UC_MATH_CALL splat_x(afloat4 value)
        {
            return swizzle<x, x, x, x>(value);
        }

        inline float4 UC_MATH_CALL splat_y(afloat4 value)
        {
            return swizzle<y, y, y, y>(value);
        }

        inline float4 UC_MATH_CALL splat_z(afloat4 value)
        {
            return swizzle<z, z, z, z>(value);
        }

        inline float4 UC_MATH_CALL splat_w(afloat4 value)
        {
            return swizzle<w, w, w, w>(value);
        }

        inline float4 UC_MATH_CALL set(float v1, float v2, float v3, float v4)
        {
            return _mm_set_ps(v4, v3, v2, v1);
        }

        inline float4 UC_MATH_CALL set_uint32(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
        {
            __m128i v = _mm_set_epi32(v4, v3, v2, v1);
            return reinterpret_cast<__m128*> (&v)[0];
        }

        inline float4 UC_MATH_CALL load1(const void* const __restrict address)
        {
            return _mm_load_ss(reinterpret_cast<const float* const __restrict> (address));
        }

        inline float4 UC_MATH_CALL load2(const void* const address)
        {
            __m128i v = _mm_loadl_epi64((const __m128i *) address);
            return reinterpret_cast<__m128 *>(&v)[0];
        }

        inline float4 UC_MATH_CALL load3(const void* __restrict const address, uint32_t w_component)
        {
            float4 v = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, w_component);
            float4 v0 = set_uint32(0, 0, 0, w_component);
            return _mm_or_ps(_mm_and_ps(v, _mm_load_ps(reinterpret_cast<const float* __restrict> (address))), v0);
        }

        inline float4 UC_MATH_CALL load3u(const void* __restrict const address, uint32_t w_component)
        {
            float4 v  = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
            float4 v0 = set_uint32(0, 0, 0, w_component);
            return _mm_or_ps(_mm_and_ps(v, _mm_loadu_ps(reinterpret_cast<const float* __restrict> (address))), v0);
        }

        inline float4 UC_MATH_CALL load3(const void* __restrict const address)
        {
            float4 v = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
            return _mm_and_ps(v, _mm_load_ps(reinterpret_cast<const float* __restrict> (address)));
        }

        inline float4 UC_MATH_CALL load3u(const void* __restrict const address)
        {
            float4 v = set_uint32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0);
            return _mm_and_ps(v, _mm_loadu_ps(reinterpret_cast<const float* __restrict> (address)));
        }

        inline float4 UC_MATH_CALL load4(const void* __restrict const address)
        {
            return _mm_load_ps(reinterpret_cast<const float* __restrict const> (address));
        }

        inline float4 UC_MATH_CALL load4(const float* __restrict const address)
        {
            return _mm_load_ps(reinterpret_cast<const float* __restrict const> (address));
        }

        inline __m128i UC_MATH_CALL load4i(const void* __restrict const address)
        {
            return _mm_load_si128(reinterpret_cast<const __m128i * __restrict const> (address));
        }

        inline float4 UC_MATH_CALL load4u(const void* __restrict const address)
        {
            return _mm_loadu_ps(reinterpret_cast<const float* __restrict> (address));
        }

        inline float4 UC_MATH_CALL load4u(const float* __restrict const address)
        {
            return _mm_loadu_ps(reinterpret_cast<const float* __restrict> (address));
        }

        inline void UC_MATH_CALL store1(void* __restrict address, afloat4 value)
        {
            _mm_store_ss(reinterpret_cast<float*__restrict> (address), value);
        }

        inline void UC_MATH_CALL store1(float* __restrict address, afloat4 value)
        {
            _mm_store_ss(reinterpret_cast<float*__restrict> (address), value);
        }

        inline void UC_MATH_CALL store2(void* __restrict address, afloat4 value)
        {
            __m128i* __restrict destination = reinterpret_cast<__m128i* __restrict> (address);
            __m128i  v = reinterpret_cast<const __m128i* __restrict> (&value)[0];
            _mm_storel_epi64(destination, v);
        }

        inline void UC_MATH_CALL store2(float* __restrict address, afloat4 value)
        {
            __m128i* __restrict destination = reinterpret_cast<__m128i* __restrict> (address);
            __m128i  v = reinterpret_cast<const __m128i* __restrict> (&value)[0];
            _mm_storel_epi64(destination, v);
        }

        inline void UC_MATH_CALL store3(void* __restrict address, afloat4 value)
        {
            float4 v1 = splat_z(value);
            store2(address, value);
            store1(reinterpret_cast<float* __restrict> (address) + 2, v1);
        }

        inline void UC_MATH_CALL store3u(void* __restrict address, afloat4 value)
        {
            store3(address, value);
        }

        inline void UC_MATH_CALL store3(float* __restrict address, afloat4 value)
        {
            float4 v1 = splat_z(value);
            store2(address, value);
            store1(reinterpret_cast<float* __restrict> (address) + 2, v1);
        }

        inline void UC_MATH_CALL store3u(float* __restrict address, afloat4 value)
        {
            store3(address, value);
        }

        inline void UC_MATH_CALL store4(void* __restrict address, afloat4 value)
        {
            _mm_store_ps(reinterpret_cast<float* __restrict> (address), value);
        }

        inline void UC_MATH_CALL store4(float* __restrict address, afloat4 value)
        {
            _mm_store_ps(reinterpret_cast<float* __restrict> (address), value);
        }

        inline void UC_MATH_CALL stream(void* __restrict address, afloat4 value)
        {
            _mm_stream_ps(reinterpret_cast<float* __restrict> (address), value);
        }

        inline void UC_MATH_CALL stream(float* __restrict address, afloat4 value)
        {
            _mm_stream_ps(reinterpret_cast<float* __restrict> (address), value);
        }

        //compare operations

        inline float4 UC_MATH_CALL compare_eq(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpeq_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_lt(afloat4 v1, afloat4 v2)
        {
            return _mm_cmplt_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_le(afloat4 v1, afloat4 v2)
        {
            return _mm_cmple_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_gt(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpgt_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_ge(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpge_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_not_eq(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpneq_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_not_lt(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpnlt_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_not_le(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpnle_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_not_gt(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpngt_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL compare_not_ge(afloat4 v1, afloat4 v2)
        {
            return _mm_cmpnge_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL select(afloat4 value1, afloat4 value2, afloat4 control)
        {
            float4 v1 = _mm_andnot_ps(control, value1);
            float4 v2 = _mm_and_ps(value2, control);
            return _mm_or_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL select_control(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
        {
            float4 v = set_uint32(v1, v2, v3, v4);
            float4 z = zero();
            return compare_gt(v, z);
        }


        //simple math operations
        inline float4 UC_MATH_CALL add(afloat4 v1, afloat4 v2)
        {
            return _mm_add_ps(v1, v2);
        }

        //simple math operations
        inline float4 UC_MATH_CALL horizontal_add(afloat4 v1, afloat4 v2)
        {
            return _mm_hadd_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL sub(afloat4 v1, afloat4 v2)
        {
            return _mm_sub_ps(v1, v2);
        }

        //simple math operations
        inline float4 UC_MATH_CALL horizontal_sub(afloat4 v1, afloat4 v2)
        {
            return _mm_hsub_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL mul(afloat4 v1, afloat4 v2)
        {
            return _mm_mul_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL mul(afloat4 v, float f)
        {
            float4 v1 = splat(f);
            return mul(v, v1);
        }

        inline float4 UC_MATH_CALL mad(afloat4 v1, afloat4 v2, afloat4 v3)
        {
            return add(mul(v1, v2), v3);
        }

        inline float4 UC_MATH_CALL div(afloat4 v1, afloat4 v2)
        {
            return _mm_div_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL sqrt(afloat4 v)
        {
            return _mm_sqrt_ps(v);
        }

        inline float4 UC_MATH_CALL rcp(afloat4 v)
        {
            return _mm_rcp_ps(v);
        }

        inline float4 UC_MATH_CALL rsqrt(afloat4 v)
        {
            return _mm_rsqrt_ps(v);
        }

        inline float4 UC_MATH_CALL min(afloat4 v1, afloat4 v2)
        {
            return _mm_min_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL max(afloat4 v1, afloat4 v2)
        {
            return _mm_max_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL clamp(afloat4 v, afloat4 a, afloat4 b)
        {
            float4 v1 = min(v, b);
            float4 v2 = max(v1, a);
            return v2;
        }

        inline float4 UC_MATH_CALL saturate(afloat4 v)
        {
            return clamp(v, zero(), one());
        }

        inline float4 UC_MATH_CALL negate(afloat4 v)
        {
            return mul(v, minus_one());
        }

        inline float4 UC_MATH_CALL abs(afloat4 v)
        {
            float4 v3 = sub(zero(), v);
            float4 v4 = max(v, v3);
            return v4;
        }

        inline float4 UC_MATH_CALL lerp(afloat4 v1, afloat4 v2, afloat4 l)
        {
            float4 a = sub(v2, v1);
            return mad(l, a, v2);
        }

        inline float4 UC_MATH_CALL lerp(afloat4 v1, afloat4 v2, float l)
        {
            float4 s = splat(l);
            return lerp(v1, v2, s);
        }


        //simple logical operations
        inline float4 UC_MATH_CALL simd_and(afloat4 v1, afloat4 v2)
        {
            return _mm_and_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL simd_and_not(afloat4 v1, afloat4 v2)
        {
            return _mm_andnot_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL simd_or(afloat4 v1, afloat4 v2)
        {
            return _mm_or_ps(v1, v2);
        }

        inline float4 UC_MATH_CALL simd_xor(afloat4 v1, afloat4 v2)
        {
            return _mm_xor_ps(v1, v2);
        }

        //misc functions
        inline int32_t UC_MATH_CALL movemask(afloat4 v)
        {
            return _mm_movemask_ps(v);
        }

        //math functions
        inline float4 UC_MATH_CALL dot2(afloat4 v1, afloat4 v2)
        {
            float4 v3 = mul(v1, v2);
            float4 v4 = swizzle<x, x, x, x>(v3);
            float4 v5 = swizzle<y, y, y, y>(v3);
            return add(v4, v5);
        }

        inline float4 UC_MATH_CALL dot3(afloat4 v1, afloat4 v2)
        {
            float4 v3 = mul(v1, v2);
            float4 v4 = swizzle<x, x, x, x>(v3);
            float4 v5 = swizzle<y, y, y, y>(v3);
            float4 v6 = swizzle<z, z, z, z>(v3);
            float4 v7 = add(v4, v5);
            return add(v6, v7);
        }

        inline float4 UC_MATH_CALL dot4(afloat4 v1, afloat4 v2)
        {
            float4 v3 = mul(v1, v2);
            float4 v4 = horizontal_add(v3, v3);
            float4 v5 = horizontal_add(v4, v4);
            return v5;
        }

        inline float4 UC_MATH_CALL length2(afloat4 v)
        {
            float4 d = dot2(v, v);
            float4 l = sqrt(d);
            return l;
        }

        inline float4 UC_MATH_CALL length3(afloat4 v)
        {
            float4 d = dot3(v, v);
            float4 l = sqrt(d);
            return l;
        }

        inline float4 UC_MATH_CALL length4(afloat4 v)
        {
            float4 d = dot4(v, v);
            float4 l = sqrt(d);
            return l;
        }

        inline float4 UC_MATH_CALL normalize2(afloat4 v)
        {
            float4 l = length2(v);
            float4 n = div(v, l);
            return n;
        }

        inline float4 UC_MATH_CALL normalize3(afloat4 v)
        {
            float4 l = length3(v);
            float4 n = div(v, l);
            return n;
        }

        inline float4 UC_MATH_CALL normalize4(afloat4 v)
        {
            float4 l = length4(v);
            float4 n = div(v, l);
            return n;
        }

        inline float4 UC_MATH_CALL normalize_plane(afloat4 v)
        {
            float4 l = length3(v);
            float4 n = div(v, l);
            return n;
        }

        inline float4 UC_MATH_CALL cross2(afloat4 v1, afloat4 v2)
        {
            float4 v3 = swizzle<x, y, x, y>(v2);
            float4 v4 = mul(v1, v3);
            float4 v5 = swizzle<y, y, y, y>(v4);
            float4 v6 = sub(v4, v5);
            float4 v7 = swizzle<x, x, x, x>(v6);
            return v7;
        }

        inline float4 UC_MATH_CALL cross3(afloat4 v1, afloat4 v2)
        {
            float4 v3 = swizzle<y, z, x, w>(v1);
            float4 v4 = swizzle<z, x, y, w>(v2);
            float4 v5 = mul(v3, v4);

            float4 v6 = swizzle<z, x, y, w>(v1);
            float4 v7 = swizzle<y, z, x, w>(v2);

            float4 v8 = mul(v6, v7);
            float4 v9 = sub(v5, v8);

            return v9;
        }

        inline float4 UC_MATH_CALL ortho2(afloat4 v)
        {
            float4 v3 = swizzle<y, x, z, w>(v);
            float4 v4 = negate(v3);
            return v4;
        }

        inline float4 UC_MATH_CALL ortho4(afloat4 v)
        {
            float4 v3 = swizzle<y, x, w, z>(v);
            float4 v4 = negate(v3);
            return v4;
        }

        namespace details
        {
            template <uint32_t c> inline float UC_MATH_CALL get_component(afloat4 v)
            {
                float4 v1 = swizzle<c, c, c, c>(v);
                float __declspec(align(16)) f;
                store1(&f, v1);
                return f;
            }
        }

        inline float UC_MATH_CALL get_x(afloat4 v)
        {
            return details::get_component<x>(v);
        }

        inline float UC_MATH_CALL get_y(afloat4 v)
        {
            return details::get_component<y>(v);
        }

        inline float UC_MATH_CALL get_z(afloat4 v)
        {
            return details::get_component<z>(v);
        }

        inline float UC_MATH_CALL get_w(afloat4 v)
        {
            return details::get_component<w>(v);
        }

        inline float4 UC_MATH_CALL mask_x()
        {
            static const uint32_t __declspec(align(16)) mask_x[4] = { 0xFFFFFFFF, 0x0, 0x0, 0x0 };
            return load4(&mask_x[0]);
        }

        inline float4 UC_MATH_CALL mask_y()
        {
            static const uint32_t __declspec(align(16)) mask_y[4] = { 0x0, 0xFFFFFFFF, 0x0, 0x0 };
            return load4(&mask_y[0]);
        }

        inline float4 UC_MATH_CALL mask_z()
        {
            static const uint32_t __declspec(align(16)) mask_z[4] = { 0x0, 0x0, 0xFFFFFFFF, 0x0 };
            return load4(&mask_z[0]);
        }

        inline float4 UC_MATH_CALL mask_w()
        {
            static const uint32_t __declspec(align(16)) mask_w[4] = { 0x0, 0x0, 0x0, 0xFFFFFFFF };
            return load4(&mask_w[0]);
        }
    }
}

