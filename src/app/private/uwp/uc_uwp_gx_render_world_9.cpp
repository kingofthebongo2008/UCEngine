#include "pch.h"
#include "uc_uwp_gx_render_world_9.h"

#include <gsl/gsl>
#include <ppl.h>

#include <uc_dev/gx/lip/file.h>
#include <uc_dev/gx/lip_utils.h>
#include <uc_dev/gx/img_utils.h>

#include <uc_dev/gx/geo/indexed_geometry_factory.h>

#include <uc_dev/gx/dx12/gpu/texture_2d.h>

#include <autogen/shaders/indexed_mesh_main.h>
#include <autogen/shaders/depth_prepass.h>

#include "uc_uwp_gx_render_object_factory.h"
#include "uc_uwp_device_resources.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace details1
            {
                struct draw_constants
                {
                    math::float4x4                  m_world;
                };

                inline math::float4x4 military_mechanic_world_transform(double )
                {
                    math::float4x4 t1 = math::identity_matrix();
                    math::float4x4 t2 = math::identity_matrix();

                    t2.r[0] = math::identity_r0();
                    t2.r[2] = math::identity_r1();
                    t2.r[1] = math::identity_r2();
                    t2.r[3] = math::identity_r3();
                    return t1;// math::mul(t1, t0);
                }
            }

            render_world_9::render_world_9(initialize_context* c) : base(c)
            {
                concurrency::task_group g;

                //load bear texture
                g.run([this,c]()
                {
                });

                //load bear mesh
                g.run([this, c]()
                {
                    auto resources = c->m_resources;
                    auto mesh = lip::create_from_compressed_lip_file<lip::model>(L"appdata/meshes/military_mechanic_indexed.default.model");
                    m_military_mechanic = gx::geo::create_indexed_geometry(resources->resource_create_context(), resources->upload_queue(), mesh->m_positions.data(), lip::size(mesh->m_positions), mesh->m_indices.data(), lip::size(mesh->m_indices));
                });

                g.run([this, c]
                {
                    auto resources = c->m_resources;
                    m_indexed_mesh = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::indexed_mesh_main::create_pso);
                });

                g.run([this, c]
                {
                    auto resources          = c->m_resources;
                    m_indexed_mesh_depth    = gx::dx12::create_pso(resources->device_d2d12(), resources->resource_create_context(), gx::dx12::depth_prepass::create_pso);
                });

                m_camera->set_view_position(math::set(0.0, 0.0f, -5.5f, 0.0f));
                m_camera->set_far(1200.0f);

                g.wait();
            }

            render_world_9::~render_world_9()
            {

            }

            void render_world_9::do_update(update_context* )
            {
            }

            std::unique_ptr< submitable >render_world_9::do_render(render_context* ctx)
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
                graphics->set_pso(m_indexed_mesh);

                {
                    frame_constants frame;
                    frame.m_view = uc::math::transpose(uc::gx::view_matrix(camera()));
                    frame.m_perspective = uc::math::transpose(uc::gx::perspective_matrix(camera()));
                    graphics->set_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_0, frame);
                }

                {
                    //draw
                    details1::draw_constants draw;
                    draw.m_world = uc::math::transpose( details1::military_mechanic_world_transform(0.00f) );// math::identity_matrix(); //uc::math::transpose(m_military_mechanic_transform); //// 

                    //draw
                    graphics->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                    //geometry
                    graphics->set_vertex_buffer(0, gx::geo::make_position_buffer_view(&m_military_mechanic));
                    graphics->set_index_buffer(gx::geo::make_index_buffer_view(&m_military_mechanic));

                    graphics->set_dynamic_constant_buffer(gx::dx12::default_root_singature::slots::constant_buffer_1, 0, draw);

                    //Draw call -> frequency 2 ( nvidia take care these should be on a sub 1 ms granularity)
                    graphics->draw_indexed(m_military_mechanic.m_index_count);
                }

                end_render(ctx, graphics.get());

                return std::make_unique<graphics_submitable>(std::move(graphics));
            }
        }
    }
}