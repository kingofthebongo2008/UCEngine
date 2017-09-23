#pragma once

#include <tuple>

#include <uc_dev/math/vector.h>
#include <uc_dev/math/defines.h>

namespace uc
{
    namespace math
    {
        /*
        void branchlessONB(const Vec3f &n, Vec3f &b1, Vec3f &b2)
        {
            float sign = copysignf(1.0f, n.z);
            const float a = -1.0f / (sign + n.z);
            const float b = n.x * n.y * a;
            b1 = Vec3f(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
            b2 = Vec3f(b, sign + n.y * n.y * a, -n.y);
        }        */
        
        inline float4 UC_MATH_CALL orthogonal3(afloat4 v)
        {
            // see http://blog.selfshadow.com/2011/10/17/perp-vectors/

            float4 zer                  = zero();
            float4 z0                   = splat_z(v);
            float4 yzyy                 = swizzle<y, z, y, y>(v);

            float4 negative_v           = negate(v);

            float4 z_is_negative        = compare_lt(z0, zer);
            float4 yzyy_is_negative     = compare_lt(yzyy, zer);

            float4 s = add(yzyy, z0);
            float4 d = sub(yzyy, z0);

            float4 sel = compare_eq_int(z_is_negative, yzyy_is_negative);

            float4 r0 = permute<permute_1x, permute_0x, permute_0x, permute_0x>(negative_v, s);
            float4 r1 = permute<permute_1x, permute_0x, permute_0x, permute_0x>(v, d);

            return select(r1, r0, sel);
        }

        inline float4 UC_MATH_CALL orthogonal3_vector(afloat4 v)
        {
            return vector3(orthogonal3(v));
        }
    }
}

    /*

inline XMVECTOR XM_CALLCONV XMVector3Orthogonal
(
    FXMVECTOR V
)
{
    XMVECTOR Zero = XMVectorZero();
    XMVECTOR Z = XMVectorSplatZ(V);
    XMVECTOR YZYY = XMVectorSwizzle<XM_SWIZZLE_Y, XM_SWIZZLE_Z, XM_SWIZZLE_Y, XM_SWIZZLE_Y>(V);

    XMVECTOR NegativeV = XMVectorSubtract(Zero, V);

    XMVECTOR ZIsNegative = XMVectorLess(Z, Zero);
    XMVECTOR YZYYIsNegative = XMVectorLess(YZYY, Zero);

    XMVECTOR S = XMVectorAdd(YZYY, Z);
    XMVECTOR D = XMVectorSubtract(YZYY, Z);

    XMVECTOR Select = XMVectorEqualInt(ZIsNegative, YZYYIsNegative);

    XMVECTOR R0 = XMVectorPermute<XM_PERMUTE_1X, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X>(NegativeV, S);
    XMVECTOR R1 = XMVectorPermute<XM_PERMUTE_1X, XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0X>(V, D);

    return XMVectorSelect(R1, R0, Select);
}
*/

