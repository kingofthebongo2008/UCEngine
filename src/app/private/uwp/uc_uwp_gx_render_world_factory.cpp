#include "pch.h"
#include "uc_uwp_gx_render_world_factory.h"

#include "uc_uwp_gx_render_world_moment_shadow_maps.h"
#include "uc_uwp_gx_render_world_non_linear_moment_shadow_maps_64.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            std::unique_ptr<render_world> render_world_factory::create_world( world_id id, initialize_context* ctx )
            {
                switch (id)
                {
                    case world_id::moment_shadow_maps : return std::unique_ptr<render_world>(new render_world_moment_shadow_maps(ctx));
                    case world_id::non_linear_moment_shadow_maps_64: return std::unique_ptr<render_world>(new render_world_non_linear_moment_shadow_maps_64(ctx));
                    default: return nullptr;
                }
            }

         }
    }
}