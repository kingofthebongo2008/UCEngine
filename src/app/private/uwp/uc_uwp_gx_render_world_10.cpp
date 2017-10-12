#include "pch.h"
#include "uc_uwp_gx_render_world_10.h"

#include <gsl/gsl>
#include <ppl.h>
#include <array>

#include <uc_dev/gx/dx12/gpu/texture_2d.h>
#include <uc_dev/gx/dx12/cmd/profiler.h>

#include <uc_dev/gx/lip/file.h>
#include <uc_dev/gx/lip_utils.h>
#include <uc_dev/gx/img_utils.h>
#include <uc_dev/gx/anm/anm.h>

#include <autogen/shaders/textured_skinned_lit.h>
#include <autogen/shaders/depth_prepass_skinned.h>
#include <autogen/shaders/shadows_skinned.h>
#include <autogen/shaders/shadows_resolve.h>

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

            render_world_10::render_world_10(initialize_context* c) : base(c)
            {
                concurrency::task_group g;

                g.run([this, c]
                {
                    m_military_mechanic = gxu::make_render_object_from_file<skinned_render_object>(L"appdata/meshes/military_mechanic.skinned.model", c->m_resources, c->m_geometry);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_skinned_textured_lit = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::textured_skinned_lit::create_pso);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_shadows_resolve = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::shadows_resolve::create_pso);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_skinned_depth = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::depth_prepass_skinned::create_pso);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_skinned_shadows = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::shadows_skinned::create_pso);
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


                math::float4 light_direction        = math::normalize3(math::vector3(0.0, 1.0, 0.0));
                math::float4 view_forward           = m_camera->forward();
                math::float4 shadows_forward        = math::negate(light_direction);
                math::float4 shadows_up             = math::orthogonal3_vector(light_direction);

                m_shadow_camera->set_view_position(math::add( math::point3(0,0,0) , math::mul(light_direction, 5)));
                m_shadow_camera->set_forward(shadows_forward);
                m_shadow_camera->set_up(shadows_up);

                g.wait();
                m_animation_instance = std::make_unique<gx::anm::animation_instance>(m_military_mechanic_animations.get(), m_military_mechanic_skeleton.get());
            }

            render_world_10::~render_world_10()
            {

            }

            void render_world_10::do_update(update_context* ctx)
            {
                *m_military_mechanic_transform = details::military_mechanic_world_transform(ctx->m_frame_time);

                m_skeleton_instance->reset();
                m_animation_instance->accumulate(   m_skeleton_instance.get(), ctx->m_frame_time    );

                m_constants_frame.m_view = uc::math::transpose(uc::gx::view_matrix(camera()));
                m_constants_frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(camera()));


                {
                    //do a scene bounding volume, something that encompasses all meshes, should be as tight as possible. transform it to view space and make the orthogonal projection
                    math::float4 p0 = math::point3(0.0f, 0.0f, 0.0f);
                    math::float4 p1 = math::vector3(1.0f, 2.5f, 1.0f);
                    math::aabb1 scene = { p0, p1 };

                    math::euclidean_transform_3d view = math::make_euclidean_transform_3d(gx::view_matrix(m_shadow_camera.get()));
                    math::aabb1 scene_bounds_vs = math::transform(scene, view);


                    math::float4 aabb_min = math::bounds_min(scene_bounds_vs);
                    math::float4 aabb_max = math::bounds_max(scene_bounds_vs);

                    m_shadow_camera->set_x_min(math::get_x(aabb_min));
                    m_shadow_camera->set_x_max(math::get_x(aabb_max));

                    m_shadow_camera->set_y_min(math::get_y(aabb_min));
                    m_shadow_camera->set_y_max(math::get_y(aabb_max));

                    m_shadow_camera->set_z_min(math::get_z(aabb_max));
                    m_shadow_camera->set_z_max(math::get_z(aabb_min));

                    m_constants_frame_shadows.m_view        = uc::math::transpose(uc::gx::view_matrix(m_shadow_camera.get()));
                    m_constants_frame_shadows.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(m_shadow_camera.get()));
                }

                {
                    skinned_draw_constants& draw = m_constants_pass;
                    draw.m_world = uc::math::transpose(*m_military_mechanic_transform);

                    {
                        auto skeleton = m_military_mechanic_skeleton.get();
                        auto joints = gx::anm::local_to_world_joints2(skeleton, m_skeleton_instance->local_transforms());

                        for (auto i = 0U; i < joints.size(); ++i)
                        {
                            math::float4x4 bind_pose    = math::load44(&skeleton->m_joint_inverse_bind_pose2[i].m_a0);
                            math::float4x4 palette      = math::mul(bind_pose, joints[i]);
                            draw.m_joints_palette[i]    = math::transpose(palette);
                        }
                    }
                }
            }

            std::unique_ptr< submitable >render_world_10::do_render(render_context* ctx)
            {
                //now start new ones
                auto resources = ctx->m_resources;
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                auto profile_event = uc::gx::dx12::make_profile_event(graphics.get(), L"Main Pass");

                begin_render(ctx, graphics.get());

                {
                    set_view_port(ctx, graphics.get());
                    graphics->set_descriptor_heaps();
                }

                //Per many draw calls  -> frequency 1
                graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                graphics->set_pso(m_skinned_textured_lit);
                graphics->set_descriptor_heaps();

                graphics->set_graphics_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, m_constants_frame);

                //mechanic
                {
                    //todo: move this into a big buffer for the whole scene
                    graphics->set_graphics_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, m_constants_pass);

                    //geometry
                    graphics->set_vertex_buffer(0, ctx->m_geometry->skinned_mesh_position_view());
                    graphics->set_vertex_buffer(1, ctx->m_geometry->skinned_mesh_uv_view());
                    graphics->set_vertex_buffer(2, ctx->m_geometry->skinned_mesh_blend_weight_view());
                    graphics->set_vertex_buffer(3, ctx->m_geometry->skinned_mesh_blend_index_view());
                    graphics->set_index_buffer(ctx->m_geometry->indices_view());

                    size_t start = 0;
                    size_t size = m_military_mechanic->m_primitive_ranges.size();

                    for (auto i = start; i < start + size; ++i)
                    {
                        {
                            auto& t = m_military_mechanic->m_opaque_textures[i];
                            //material
                            graphics->set_graphics_dynamic_descriptor(gx::dx12::default_root_singature::slots::srv_1, t->srv());
                        }

                        {
                            auto& r = m_military_mechanic->m_primitive_ranges[i];
                            auto  base_index_offset = m_military_mechanic->m_indices->index_offset();
                            auto  base_vertex_offset = m_military_mechanic->m_geometry->draw_offset();

                            //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                            graphics->draw_indexed(r.size(), r.m_begin + base_index_offset, base_vertex_offset);
                        }
                    }
                }

                end_render(ctx, graphics.get());

                return std::make_unique<graphics_submitable>(std::move(graphics));
            }

            std::unique_ptr< submitable >render_world_10::do_render_depth(render_context* ctx)
            {
                //now start new ones
                auto resources = ctx->m_resources;
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                auto profile_event = uc::gx::dx12::make_profile_event(graphics.get(), L"Depth Pass");

                begin_render_depth(ctx, graphics.get());

                {
                    set_view_port(ctx, graphics.get());
                    graphics->set_descriptor_heaps();
                }

                //Per many draw calls  -> frequency 1
                graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                graphics->set_pso(m_skinned_depth);
                graphics->set_descriptor_heaps();

                graphics->set_graphics_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, m_constants_frame);

                //mechanic
                {
                    //todo: move this into a big buffer for the whole scene
                    graphics->set_graphics_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, m_constants_pass);

                    //geometry
                    graphics->set_vertex_buffer(0, ctx->m_geometry->skinned_mesh_position_view());
                    graphics->set_vertex_buffer(1, ctx->m_geometry->skinned_mesh_blend_weight_view());
                    graphics->set_vertex_buffer(2, ctx->m_geometry->skinned_mesh_blend_index_view());
                    graphics->set_index_buffer(ctx->m_geometry->indices_view());

                    //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                    graphics->draw_indexed(m_military_mechanic->m_indices->index_count(), m_military_mechanic->m_indices->index_offset(), m_military_mechanic->m_geometry->draw_offset());
                }

                end_render_depth(ctx, graphics.get());

                return std::make_unique<graphics_submitable>(std::move(graphics));
            }

            std::unique_ptr< submitable >render_world_10::do_render_shadows(shadow_render_context* ctx)
            {
                //now start new ones
                auto resources      = ctx->m_resources;
                auto graphics       = create_graphics_compute_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                
                {
                    auto profile_event = uc::gx::dx12::make_profile_event(graphics.get(), L"Shadows Pass");

                    begin_render_shadows(ctx, graphics.get());

                    {
                        set_view_port(ctx, graphics.get());
                    }

                    //Per many draw calls  -> frequency 1
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    graphics->set_pso(m_skinned_shadows);
                    graphics->set_descriptor_heaps();

                    graphics->set_graphics_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, m_constants_frame_shadows);

                    //mechanic
                    {
                        //todo: move this into a big buffer for the whole scene
                        graphics->set_graphics_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, m_constants_pass);

                        //geometry
                        graphics->set_vertex_buffer(0, ctx->m_geometry->skinned_mesh_position_view());
                        graphics->set_vertex_buffer(1, ctx->m_geometry->skinned_mesh_blend_weight_view());
                        graphics->set_vertex_buffer(2, ctx->m_geometry->skinned_mesh_blend_index_view());
                        graphics->set_index_buffer(ctx->m_geometry->indices_view());

                        //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                        graphics->draw_indexed(m_military_mechanic->m_indices->index_count(), m_military_mechanic->m_indices->index_offset(), m_military_mechanic->m_geometry->draw_offset());
                    }

                    end_render_shadows(ctx, graphics.get());
                }
                

                {

                    graphics->transition_resource(ctx->m_shadow_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
                    graphics->transition_resource(ctx->m_shadow_map, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

                    {
                        auto profile_event = uc::gx::dx12::make_profile_event(graphics.get(), L"Shadows Resolve Pass");

                        graphics->set_pso(m_shadows_resolve);

                        graphics->set_compute_dynamic_descriptor(gx::dx12::default_root_singature::slots::srv_1, ctx->m_shadow_depth_buffer->srv_depth());
                        graphics->set_compute_dynamic_descriptor(gx::dx12::default_root_singature::slots::uav_1, ctx->m_shadow_map->uav());

                        graphics->dispatch( 2048 / 16, 2048 / 16, 1);
                    }

                    graphics->transition_resource(ctx->m_shadow_map, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
                    graphics->transition_resource(ctx->m_shadow_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
                }

                return std::make_unique<graphics_compute_submitable>(std::move(graphics));
            }
            
        }
    }
}