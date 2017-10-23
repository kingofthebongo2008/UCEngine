#pragma once

#include <memory>
#include <vector>

#include <uc_dev/mem/align.h>
#include <uc_dev/gx/dx12/dx12.h>
#include <uc_dev/gx/geo/indexed_geometry.h>
#include <uc_dev/gx/anm/animation_instance.h>
#include <uc_dev/gx/structs.h>

#include "uc_uwp_gx_render_world.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            class render_world_2 : public render_world
            {

                using base = render_world;

            public:

                render_world_2(initialize_context* ctx);
                virtual ~render_world_2();

            private:

                void do_update(update_context* ctx) override;
                std::unique_ptr< submitable > do_render(render_context* ctx) override;
                

                std::unique_ptr<gxu::skinned_multi_material_render_object>      m_military_mechanic;

                gx::dx12::graphics_pipeline_state*                              m_textured_skinned;
                gx::dx12::graphics_pipeline_state*                              m_textured_skinned_depth;

                lip::unique_lip_pointer<lip::skeleton>                          m_military_mechanic_skeleton;
                lip::unique_lip_pointer<lip::joint_animations>                  m_military_mechanic_animations;

                std::unique_ptr< gx::anm::skeleton_instance >                   m_skeleton_instance;
                std::unique_ptr< gx::anm::animation_instance>                   m_animation_instance;

                //update state
                math::managed_float4x4                                          m_military_mechanic_transform = math::make_float4x4();

                gx::dx12::graphics_pipeline_state*                              m_skeleton_pso;
                std::vector<gx::position_3d>                                    m_skeleton_positions;
            };
        }
    }
}

