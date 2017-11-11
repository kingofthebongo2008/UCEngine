#pragma once

#include <memory>
#include <vector>

#include <uc_dev/mem/align.h>
#include <uc_dev/gx/dx12/dx12.h>
#include <uc_dev/gx/blue_noise/moment_shadow_maps_blue_noise.h>
#include <uc_dev/gx/geo/indexed_geometry.h>
#include <uc_dev/gx/anm/animation_instance.h>
#include <uc_dev/gx/structs.h>

#include "uc_uwp_gx_render_world_moment_shadows_data.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            struct render_object_frame_constants
            {

                math::float4x4 m_shadow_view;
                math::float4x4 m_shadow_perspective;
                math::float4   m_directional_light;
            };

            class render_world_10 : public render_world_moment_shadows_data
            {

                using base = render_world_moment_shadows_data;

            public:

                render_world_10(initialize_context* ctx);
                virtual ~render_world_10();

            private:

                std::unique_ptr< submitable >do_render(render_context* ctx) override;
                std::unique_ptr< submitable >do_render_shadows(shadow_render_context* ctx) override;

                std::unique_ptr<gx::blue_noise::ldr_rg01_64x64> m_blue_noise;
            };
        }
    }
}

