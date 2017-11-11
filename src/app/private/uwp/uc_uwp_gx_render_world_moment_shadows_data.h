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
            class render_world_moment_shadows_data : public render_world
            {

                using base = render_world;

            public:

                render_world_moment_shadows_data(initialize_context* ctx);
                virtual ~render_world_moment_shadows_data();

            protected:

                void do_update(update_context* ctx) override;
                std::unique_ptr< submitable > do_render_depth(render_context* ctx) override;
                void on_resize_buffers(device_resources* resources) override;

                gx::dx12::gpu_msaa_depth_buffer*    on_get_shadow_depth_buffer() override;
                gx::dx12::gpu_color_buffer*         on_get_shadow_map() override;

                std::unique_ptr<gx::dx12::gpu_msaa_depth_buffer>                m_shadow_depth_buffer;
                std::unique_ptr<gx::dx12::gpu_color_buffer>                     m_shadow_map;

                std::unique_ptr<gxu::skinned_render_object>                     m_military_mechanic;

                gx::dx12::graphics_pipeline_state*                              m_skinned;
                gx::dx12::graphics_pipeline_state*                              m_skinned_depth;
                gx::dx12::graphics_pipeline_state*                              m_skinned_shadows;

                gx::dx12::graphics_pipeline_state*                              m_plane;
                gx::dx12::graphics_pipeline_state*                              m_plane_depth;
                gx::dx12::graphics_pipeline_state*                              m_plane_shadows;

                lip::unique_lip_pointer<lip::skeleton>                          m_military_mechanic_skeleton;
                lip::unique_lip_pointer<lip::joint_animations>                  m_military_mechanic_animations;

                std::unique_ptr< gx::anm::skeleton_instance >                   m_skeleton_instance;
                std::unique_ptr< gx::anm::animation_instance>                   m_animation_instance;

                //update state
                math::managed_float4x4                                          m_military_mechanic_transform = math::make_float4x4();

                gx::dx12::compute_pipeline_state*                               m_shadows_resolve;
                mem::aligned_unique_ptr<gx::orthographic_camera>                m_shadow_camera = mem::make_aligned_unique_ptr<gx::orthographic_camera>();

                struct skinned_draw_constants
                {
                    math::float4x4                  m_world;
                    std::array<math::float4x4, 127> m_joints_palette;

                    skinned_draw_constants()
                    {
                        m_world = math::identity_matrix();

                        for (auto&& i : m_joints_palette)
                        {
                            i = math::identity_matrix();
                        }
                    }
                };


                //per frame and per pass constants
                frame_constants        m_constants_frame;
                frame_constants        m_constants_frame_shadows;
                skinned_draw_constants m_constants_pass;

                math::float4           m_light_direction;

                //light
            };
        }
    }
}

