#pragma once

#include <tuple>
#include <uc_dev/gx/error.h>
#include <uc_dev/math/math.h>

#include <fbxsdk.h>

namespace uc
{
    namespace gx
    {
        namespace import
        {
            namespace fbx
            {
                inline fbxsdk::FbxMatrix negate_y_fbx()
                {
                    fbxsdk::FbxMatrix m;

                    m.SetColumn(0, fbxsdk::FbxVector4(1, 0, 0, 0));
                    m.SetColumn(1, fbxsdk::FbxVector4(0, -1, 0, 0));
                    m.SetColumn(2, fbxsdk::FbxVector4(0, 0, 1, 0));
                    m.SetColumn(3, fbxsdk::FbxVector4(0, 0, 0, 1));

                    return m;
                }

                inline math::float4x4 negate_y_math()
                {
                    math::float4x4 m;

                    m.r[0] = math::identity_r0();
                    m.r[1] = math::negate(math::identity_r1());
                    m.r[2] = math::identity_r2();
                    m.r[3] = math::identity_r3();

                    return m;
                }

                inline fbxsdk::FbxMatrix negate_x_fbx()
                {
                    fbxsdk::FbxMatrix m;

                    m.SetColumn(0, fbxsdk::FbxVector4(-1, 0, 0, 0));
                    m.SetColumn(1, fbxsdk::FbxVector4(0, 1, 0, 0));
                    m.SetColumn(2, fbxsdk::FbxVector4(0, 0, 1, 0));
                    m.SetColumn(3, fbxsdk::FbxVector4(0, 0, 0, 1));

                    return m;
                }

                inline math::float4x4 negate_x_math()
                {
                    math::float4x4 m;

                    m.r[0] = math::negate(math::identity_r0());
                    m.r[1] = math::identity_r1();
                    m.r[2] = math::identity_r2();
                    m.r[3] = math::identity_r3();

                    return m;
                }

                inline fbxsdk::FbxMatrix swap_y_z_fbx()
                {
                    fbxsdk::FbxMatrix m;

                    m.SetColumn(0, fbxsdk::FbxVector4(1, 0, 0, 0));
                    m.SetColumn(1, fbxsdk::FbxVector4(0, 0, 1, 0));
                    m.SetColumn(2, fbxsdk::FbxVector4(0, 1, 0, 0));
                    m.SetColumn(3, fbxsdk::FbxVector4(0, 0, 0, 1));

                    return m;
                }

                inline math::float4x4 swap_y_z_math()
                {
                    math::float4x4 m;

                    m.r[0] = math::identity_r0();
                    m.r[1] = math::identity_r2();
                    m.r[2] = math::identity_r1();
                    m.r[3] = math::identity_r3();

                    return m;
                }


                inline fbxsdk::FbxVector4 swap_y_z_vector(fbxsdk::FbxVector4 v)
                {
                    auto swap = swap_y_z_fbx();
                    auto t0 = swap.MultNormalize(v);
                    return t0;
                }

                inline fbxsdk::FbxVector4 swap_y_z_point(fbxsdk::FbxVector4 v)
                {
                    auto swap = swap_y_z_fbx();
                    auto t0 = swap.MultNormalize(v);
                    return t0;
                }

                inline fbxsdk::FbxAMatrix swap_y_z_matrix( const fbxsdk::FbxAMatrix v )
                {
                    fbxsdk::FbxAMatrix  m  = v;
                    fbxsdk::FbxVector4  tr = v.GetT();

                    m.SetT(fbxsdk::FbxVector4(0.0, 0.0, 0.0, 1.f));
                    fbxsdk::FbxMatrix   rot = m;

                    auto swap  = swap_y_z_fbx();
                    auto neg_y = negate_y_fbx();

                    auto t0 = swap.MultNormalize(tr);
                    auto t1 = t0;// neg_y.MultNormalize(t0);

                    auto r0 = swap.Transpose()   * rot * swap;
                    auto r1 = r0;// neg_y.Transpose()  * rot * neg_y;

                    fbxsdk::FbxAMatrix r;
                    auto dst = static_cast<double*>(r);
                    auto src = static_cast<double*>(r1);

                    std::memcpy(dst, src, sizeof(fbxsdk::FbxAMatrix));

                    r.SetT(t1);

                    return r;
                }

                inline math::float4 swap_y_z_vector(math::afloat4 v)
                {
                    auto swap   = swap_y_z_math();
                    auto t0     = math::mul(v,swap);
                    return t0;
                }

                inline math::float4 swap_y_z_point(math::afloat4 v)
                {
                    auto swap = swap_y_z_math();
                    auto t0 = math::mul(v, swap);
                    return t0;
                }

                inline math::float4x4 swap_y_z_matrix(math::afloat4x4 v)
                {
                    auto m  = v;
                    auto tr = v.r[3];

                    m.r[3] = math::identity_r3();

                    auto   rot = v;

                    auto swap  = swap_y_z_math();
                    auto neg_y = negate_y_math();

                    auto t0 = math::mul(tr, swap);
                    auto t1 = t0;// neg_y.MultNormalize(t0);

                    auto r0 = math::mul( math::mul(swap, rot ), math::transpose(swap));
                    auto r1 = r0;// neg_y.Transpose()  * rot * neg_y;

                    auto r = r0;
                    r.r[3] = t1;

                    return r;
                }

            }
        }
    }
}


