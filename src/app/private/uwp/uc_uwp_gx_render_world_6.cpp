#include "pch.h"
#include "uc_uwp_gx_render_world_6.h"

#include <gsl/gsl>
#include <ppl.h>

#include <uc_dev/gx/geo/indexed_geometry_factory.h>
#include <uc_dev/gx/dx12/gpu/texture_2d.h>

#include <uc_dev/gx/lip/file.h>
#include <uc_dev/gx/lip_utils.h>
#include <uc_dev/gx/img_utils.h>


#include <autogen/shaders/textured_solid.h>
#include <autogen/shaders/textured_depth_only.h>

#include "uc_uwp_device_resources.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace details
            {
                inline math::float4x4 robot_world_transform(double frame_time)
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

            render_world_6::render_world_6(initialize_context* c) : base(c)
            {
                concurrency::task_group g;

                g.run([this, c]
                {
                    auto mesh = lip::create_from_compressed_lip_file<lip::multi_textured_model>(L"appdata/meshes/robot_mid_poly.multi_textured.model");
                    auto resources = c->m_resources;

                    auto span_uv        = gsl::make_span(mesh->m_uv.data(), mesh->m_uv.size());
                    auto span_indices   = gsl::make_span(mesh->m_indices.data(), mesh->m_indices.size());
                    auto span_positions = gsl::make_span(mesh->m_positions.data(), mesh->m_positions.size());

                    m_robot = gx::geo::create_multi_material_geometry(resources->resource_create_context(), resources->upload_queue(), gsl::as_bytes(span_positions), gsl::as_bytes(span_indices), gsl::as_bytes(span_uv), mesh->m_primitive_ranges);

                    m_textures_robot.resize(mesh->m_textures.size());

                    for (auto i = 0U; i < mesh->m_textures.size(); ++i)
                    {
                        auto& texture = mesh->m_textures[i];

                        auto w = texture.m_width;
                        auto h = texture.m_height;

                        m_textures_robot[i] = gx::dx12::create_texture_2d(resources->resource_create_context(), w, h, static_cast<DXGI_FORMAT>(texture.view()));
                        D3D12_SUBRESOURCE_DATA s = gx::sub_resource_data(&texture);
                        resources->upload_queue()->upload_texture_2d(m_textures_robot[i].get(), 0, 1, &s);
                    }
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_textured = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::textured_solid::create_pso);
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_textured_depth = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::textured_depth_only::create_pso);
                });
             
                m_camera->set_view_position(math::set(0.0, 0.0f, -25.5f, 0.0f));
                m_camera->set_far(1200.0f);

                g.wait();
            }

            render_world_6::~render_world_6()
            {

            }

            void render_world_6::do_update( update_context* ctx )
            {
                *m_robot_transform = details::robot_world_transform(ctx->m_frame_time);
            }

            std::unique_ptr< submitable >render_world_6::do_render( render_context* ctx )
            {
                //now start new ones
                auto resources      = ctx->m_resources;
                auto graphics       = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                begin_render(ctx, graphics.get() );

                {
                    set_view_port(ctx, graphics.get()) ;
                    graphics->set_descriptor_heaps();
                }

                //Per many draw calls  -> frequency 1
                graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                graphics->set_pso(m_textured);

                {
                    frame_constants frame;
                    frame.m_view = uc::math::transpose(uc::gx::view_matrix(camera()));
                    frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(camera()));
                    graphics->set_graphics_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, frame);
                }

                //robot
                {
                    //draw
                    draw_constants draw;
                    draw.m_world = uc::math::transpose(*m_robot_transform);

                    graphics->set_pso(m_textured);
                    graphics->set_descriptor_heaps();

                    //todo: move this into a big buffer for the whole scene
                    graphics->set_graphics_dynamic_constant_buffer( gx::dx12::default_root_singature::slots::constant_buffer_1, 0, draw );
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    
                    //geometry
                    graphics->set_vertex_buffer(0, gx::geo::make_position_buffer_view(&m_robot));
                    graphics->set_vertex_buffer(1, gx::geo::make_uv_buffer_view(&m_robot));

                    graphics->set_index_buffer(gx::geo::make_index_buffer_view(&m_robot));

                    size_t start = 0;
                    size_t size = m_robot.m_ranges.size();

                    for (auto i = start; i < start + size; ++i)
                    {
                        {
                            auto& t = m_textures_robot[i];
                            //material
                            graphics->set_graphics_dynamic_descriptor(gx::dx12::default_root_singature::slots::srv_1, t->srv());
                        }

                        {
                            auto& r = m_robot.m_ranges[i];
                            //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                            graphics->draw_indexed(r.size(), r.m_begin);
                        }
                    }
                }

                end_render(ctx, graphics.get());

                return std::make_unique<graphics_submitable>(std::move(graphics));
            }

            std::unique_ptr< submitable >render_world_6::do_render_depth(render_context* ctx)
            {
                auto resources = ctx->m_resources;
                //now start new ones
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));
                begin_render_depth(ctx, graphics.get());

                {
                    set_view_port(ctx, graphics.get());
                    graphics->set_descriptor_heaps();
                }

                //Per many draw calls  -> frequency 1
                graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                graphics->set_pso(m_textured_depth);

                {
                    frame_constants frame;
                    frame.m_view = uc::math::transpose(uc::gx::view_matrix(camera()));
                    frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(camera()));
                    graphics->set_graphics_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, frame);
                }

                //robot
                {
                    //draw
                    draw_constants draw;
                    draw.m_world = uc::math::transpose(*m_robot_transform);

                    //todo: move this into a big buffer for the whole scene
                    graphics->set_graphics_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, draw);
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                    //geometry
                    graphics->set_vertex_buffer(0, gx::geo::make_position_buffer_view(&m_robot));
                    graphics->set_index_buffer(gx::geo::make_index_buffer_view(&m_robot));

                    //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                    graphics->draw_indexed(m_robot.indexes().size());
                }

                end_render_depth(ctx, graphics.get());
                return std::make_unique<graphics_submitable>(std::move(graphics));
            }
        }
    }
}