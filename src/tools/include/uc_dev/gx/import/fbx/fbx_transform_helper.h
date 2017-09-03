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
                math::float4x4 transform_from_maya(math::afloat4x4 m)
                {
                    return m;
                }

                math::float4 transform_from_maya(math::afloat4 m)
                {
                    return m;
                }

                math::float4x4 transform_from_max(math::afloat4x4 m)
                {
                    return m;
                }

                math::float4 transform_from_max(math::afloat4 m)
                {
                    return m;
                }

                math::float4x4 transform_from_dcc(math::afloat4x4 m, const fbx_context* ctx)
                {
                    ctx;
                    return m;
                }

                math::float4 transform_from_dcc(math::afloat4 m, const fbx_context* ctx)
                {
                    ctx;
                    return m;
                }

                std::array<int32_t, 3> triangle_permuation(const fbx_context* ctx)
                {
                    ctx;
                    std::array<int32_t, 3> r = { 0,1,2 };
                    return r;
                }

            }
        }
    }
}


