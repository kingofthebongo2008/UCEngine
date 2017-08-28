#pragma once

#include <tuple>
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
                inline fbxsdk::FbxVector4 swap_y_z_vector(fbxsdk::FbxVector4 v, const fbx_context* c )
                {
                    if (c->m_swap_y_z)
                    {
                        return swap_y_z_vector(v);
                    }
                    else
                    {
                        return v;
                    }
                }

                inline fbxsdk::FbxVector4 swap_y_z_point(fbxsdk::FbxVector4 v, const fbx_context* c)
                {
                    if (c->m_swap_y_z)
                    {
                        return swap_y_z_point(v);
                    }
                    else
                    {
                        return v;
                    }
                }

                inline fbxsdk::FbxAMatrix swap_y_z_matrix( const fbxsdk::FbxAMatrix v, const fbx_context* c )
                {
                    if (c->m_swap_y_z)
                    {
                        return swap_y_z_matrix(v);
                    }
                    else
                    {
                        return v;
                    }
                }
            }
        }
    }
}


