#pragma once

#include <memory>
#include <vector>

#include <uc_dev/gx/dx12/dx12.h>
#include <uc_dev/gx/geo/indexed_geometry.h>

#include "uc_uwp_gx_render_world.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            class render_world_9 : public render_world
            {
                using base = render_world;

            public:

                render_world_9(initialize_context* ctx);
                virtual ~render_world_9();

            private:

                void do_update(update_context* ctx) override;
                std::unique_ptr< submitable >do_render(render_context* ctx) override;


                gx::geo::indexed_geometry                                       m_military_mechanic;

                gx::dx12::graphics_pipeline_state*                              m_indexed_mesh;
                gx::dx12::graphics_pipeline_state*                              m_indexed_mesh_depth;
            };
        }
    }
}

