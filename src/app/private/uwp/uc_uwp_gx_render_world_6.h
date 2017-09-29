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
            class render_world_6 : public render_world
            {
                using base = render_world;

                public:

                render_world_6(initialize_context* ctx);
                virtual ~render_world_6();

            private:

                void do_update(update_context* ctx) override;
                std::unique_ptr< submitable >do_render(render_context* ctx) override;
                std::unique_ptr< submitable >do_render_depth(render_context* ctx) override;

                std::vector<gx::dx12::managed_gpu_texture_2d>                   m_textures_robot;
                gx::geo::multi_material_geometry                                m_robot;

                gx::dx12::graphics_pipeline_state*                              m_textured;
                gx::dx12::graphics_pipeline_state*                              m_depth_prepass;


				math::managed_float4x4                                          m_robot_transform = math::make_float4x4();
            };
        }
    }
}

