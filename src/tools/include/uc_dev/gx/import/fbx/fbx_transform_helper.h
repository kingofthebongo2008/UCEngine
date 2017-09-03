#pragma once

#include <tuple>
#include <array>
#include <uc_dev/gx/error.h>
#include <uc_dev/math/math.h>

#include <uc_dev/gx/import/fbx/fbx_common.h>
#include <uc_dev/gx/import/fbx/fbx_transform.h>

namespace uc
{
    namespace gx
    {
        namespace import
        {
            namespace fbx
            {

                math::float4x4 transform_from_dcc(math::afloat4x4 m, const fbx_context* ctx)
                {
                    auto m0 = swap_y_z();
                    auto m1 = negate_x();
                    auto m2 = negate_y();
                    auto m3 = negate_z();
                    auto m4 = math::identity_matrix();

                    return transform_transform(transform_transform(transform_transform(m, m3), m2), m1);
                    
                }

                math::float4 transform_from_dcc(math::afloat4 m, const fbx_context* ctx)
                {
                    auto m0 = swap_y_z();
                    auto m1 = negate_x();
                    auto m2 = negate_y();
                    auto m3 = negate_z();
                    auto m4 = math::identity_matrix();
                    return transform_vector(transform_vector(transform_vector(m, m3), m2), m1);
                }

                std::array<int32_t, 3> triangle_permuation(const fbx_context* ctx)
                {
                    ctx;
                    std::array<int32_t, 3> r = { 0, 1, 2 };
                    return r;
                }

            }
        }
    }
}


