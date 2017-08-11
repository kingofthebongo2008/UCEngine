#include "pch.h"
#include "uc_uwp_gx_render_world_2.h"

#include <gsl/gsl>
#include <ppl.h>
#include <array>

#include <uc_dev/gx/dx12/gpu/texture_2d.h>

#include <uc_dev/gx/lip/file.h>
#include <uc_dev/gx/lip_utils.h>
#include <uc_dev/gx/img_utils.h>
#include <uc_dev/gx/anm/anm.h>

#include <autogen/shaders/textured_skinned.h>
#include <autogen/shaders/depth_prepass.h>
#include <autogen/shaders/skeleton.h>

#include "uc_uwp_gx_render_object_factory.h"
#include "uc_uwp_device_resources.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace details
            {
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

                inline math::float4x4 military_mechanic_world_transform(double frame_time)
                {
                    //todo: move this in the update pahse
                    const float total = 3.14159265358979323846f * 2.0f;
                    const  uint32_t steps = 144;

                    const float step_rotation = total / steps;
                    static uint32_t step = 0;

                    static float angle = 0.0f;

                    //const float rotation_angle = (step)* step_rotation;

                    const float rotation_angle = static_cast<float>(angle + 100.0f * frame_time * step_rotation);
                    angle = rotation_angle;

                    math::float4x4 t0 = math::translation(-8.0f, 15.0f, 0.0f);
                    math::float4x4 t1 = math::rotation_y(rotation_angle);
                    math::float4x4 t2 = math::translation(-30.0f, -15.0f, 0.0f);

                    return t1;// math::mul(t1, t0);
                }
            }

            render_world_2::render_world_2(initialize_context* c) : base(c)
            {
                concurrency::task_group g;

                g.run([this, c]
                {
                    m_military_mechanic = gxu::make_render_object_from_file<gxu::skinned_multi_material_render_object>(L"appdata/meshes/military_mechanic.skinned.model", c->m_resources);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_textured_skinned = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::textured_skinned::create_pso);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_skeleton_pso = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::skeleton::create_pso);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_depth_prepass = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::depth_prepass::create_pso);
                });

                //load preprocessed textured model
                g.run([this]()
                {
                    m_military_mechanic_skeleton    = lip::create_from_compressed_lip_file<lip::skeleton>(L"appdata/skeletons/military_mechanic.skeleton");
                    m_skeleton_instance             = std::make_unique<gx::anm::skeleton_instance>(m_military_mechanic_skeleton.get());
                });

                //load preprocessed textured model
                g.run([this]()
                {
                    m_military_mechanic_animations = lip::create_from_compressed_lip_file<lip::joint_animations>(L"appdata/animations/military_mechanic.animation");
                });

                m_camera->set_view_position(math::set(0.0, 0.0f, -5.5f, 0.0f));
                m_camera->set_far(1200.0f);

                g.wait();

                m_animation_instance = std::make_unique<gx::anm::animation_instance>(m_military_mechanic_animations.get(), m_military_mechanic_skeleton.get());
                m_skeleton_positions.resize(3);
            }

            render_world_2::~render_world_2()
            {

            }

            void render_world_2::do_update(update_context* ctx)
            {
                *m_military_mechanic_transform = details::military_mechanic_world_transform(ctx->m_frame_time);

                m_skeleton_instance->reset();
                m_animation_instance->accumulate(   m_skeleton_instance.get(), ctx->m_frame_time    );
                m_skeleton_positions = gx::anm::skeleton_positions(m_military_mechanic_skeleton.get(), m_skeleton_instance->local_transforms());
            }

            gx::dx12::managed_graphics_command_context render_world_2::do_render(render_context* ctx)
            {
                //now start new ones
                auto resources = ctx->m_resources;
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                begin_render(ctx, graphics.get());

                {
                    set_view_port(ctx, graphics.get());
                    graphics->set_descriptor_heaps();
                }

                //Per many draw calls  -> frequency 1
                graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                graphics->set_pso(m_textured_skinned);

                {
                    frame_constants frame;

                    frame.m_view        = uc::math::transpose(uc::gx::view_matrix(camera()));
                    frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(camera()));
                    graphics->set_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, frame);
                }

                //robot
                {
                    //draw
                    details::skinned_draw_constants draw;
                    draw.m_world = math::identity_matrix(); //uc::math::transpose(m_military_mechanic_transform); //// 

                    {
                        auto skeleton = m_military_mechanic_skeleton.get();
                        auto joints   = gx::anm::local_to_world_joints2( skeleton, m_skeleton_instance->local_transforms() );
                        
                        for (auto i = 0U; i < joints.size(); ++i )
                        {
                            math::float4x4 bind_pose = math::load44(&skeleton->m_joint_inverse_bind_pose2[i].m_a0);
                            math::float4x4 palette   = math::mul(bind_pose, joints[i]  ); // math::mul(bind_pose, joints[i]);

                           
                            draw.m_joints_palette[i] = math::transpose(palette);
                            //draw.m_joints_palette[i] = math::transpose(math::mul(math::swap_y_z(), palette));
                        }
                    }

                    graphics->set_pso(m_textured_skinned);
                    graphics->set_descriptor_heaps();

                    //todo: move this into a big buffer for the whole scene
                    graphics->set_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, draw);
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                    //geometry
                    graphics->set_vertex_buffer(0, gx::geo::make_position_buffer_view(&m_military_mechanic->m_geometry));
                    graphics->set_vertex_buffer(1, gx::geo::make_uv_buffer_view(&m_military_mechanic->m_geometry));
                    graphics->set_vertex_buffer(2, gx::geo::make_blend_weights_buffer_view(&m_military_mechanic->m_geometry));
                    graphics->set_vertex_buffer(3, gx::geo::make_blend_indices_buffer_view(&m_military_mechanic->m_geometry));
                    graphics->set_index_buffer(gx::geo::make_index_buffer_view(&m_military_mechanic->m_geometry));

                    size_t start = 0;
                    size_t size = m_military_mechanic->m_geometry.m_ranges.size();

                    for (auto i = start; i < start + size; ++i)
                    {
                        {
                            auto& t = m_military_mechanic->m_opaque_textures[i];
                            //material
                            graphics->set_dynamic_descriptor(gx::dx12::default_root_singature::slots::srv_1, t->srv());
                        }

                        {
                            auto& r = m_military_mechanic->m_geometry.m_ranges[i];
                            //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                            graphics->draw_indexed(r.size(), r.m_begin);
                        }
                    }
                }


                {
                    //Per many draw calls  -> frequency 1
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
                    graphics->set_pso(m_skeleton_pso);

                    {
                        frame_constants frame;
                        frame.m_view = uc::math::transpose(uc::gx::view_matrix(camera()));
                        frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(camera()));
                        graphics->set_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, frame);
                    }

                    {
                        //draw
                        draw_constants draw;
                        draw.m_world = uc::math::identity_matrix();// uc::math::transpose(m_skeleton_positions_transform);

                        //todo: move this into a big buffer for the whole scene
                        graphics->set_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, draw);

                        graphics->set_dynamic_vertex_buffer(0, sizeof(gx::position_3d), resources->upload_queue()->upload_buffer(&m_skeleton_positions[0], m_skeleton_positions.size() * sizeof(gx::position_3d)));
                        graphics->draw(static_cast<uint32_t>(m_skeleton_positions.size()));
                    }
                }

                end_render(ctx, graphics.get());

                return graphics;
            }

            gx::dx12::managed_graphics_command_context render_world_2::do_render_depth(render_context* ctx)
            {

                auto resources = ctx->m_resources;
                //now start new ones
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));
                begin_render_depth(ctx, graphics.get());

                /*
                {
                    set_view_port(ctx, graphics.get());
                    graphics->set_descriptor_heaps();
                }

                //Per many draw calls  -> frequency 1
                graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                graphics->set_pso(m_depth_prepass);

                {
                    frame_constants frame;
                    frame.m_view = uc::math::transpose(uc::gx::view_matrix(m_camera));
                    frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(m_camera));
                    graphics->set_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, frame);
                }

                //robot
                {
                    //draw
                    draw_constants draw;
                    draw.m_world = uc::math::transpose(m_military_mechanic_transform);

                    //todo: move this into a big buffer for the whole scene
                    graphics->set_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, draw);
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                    //geometry
                    graphics->set_vertex_buffer(0, gx::geo::position_buffer_view(&m_military_mechanic->m_geometry));
                    graphics->set_index_buffer(gx::geo::index_buffer_view(&m_military_mechanic->m_geometry));

                    //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                    graphics->draw_indexed(m_military_mechanic->m_geometry.indexes().size());
                }
                */
                end_render_depth(ctx, graphics.get());
                return graphics;

            }
        }
    }
}