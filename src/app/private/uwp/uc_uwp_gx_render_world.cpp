#include "pch.h"
#include "uc_uwp_gx_render_world.h"

#include <uc_dev/gx/dx12/gpu/pixel_buffer.h>
#include <uc_dev/gx/dx12/cmd/profiler.h>

#include "uc_uwp_device_resources.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            D3D12_VIEWPORT viewport(const gx::dx12::gpu_pixel_buffer* b)
            {
                D3D12_VIEWPORT v = {};
                auto desc = b->desc();
                v.Width = static_cast<float>(desc.Width);
                v.Height = static_cast<float>(desc.Height);
                v.MinDepth = 0.0f;
                v.MaxDepth = 1.0f;
                return v;
            }

            D3D12_RECT scissor(const gx::dx12::gpu_pixel_buffer* b)
            {
                D3D12_RECT r = {};
                auto desc = b->desc();
                r.right = static_cast<LONG>(desc.Width);
                r.bottom = static_cast<LONG>(desc.Height);
                return r;
            }


            render_world::render_world(initialize_context*)
            {

            }

            render_world::~render_world()
            {

            }

            void render_world::update(update_context* ctx)
            {
                do_update(ctx);
            }

            gx::dx12::managed_graphics_command_context render_world::render(render_context* ctx)
            {
                return do_render(ctx);
            }

            gx::dx12::managed_graphics_command_context render_world::render_depth(render_context* ctx)
            {
                return do_render_depth(ctx);
            }

            gx::dx12::managed_graphics_command_context render_world::render_shadows(shadow_render_context* ctx)
            {
                return do_render_shadows(ctx);
            }

            void render_world::begin_render( render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics )
            {
                auto resources = ctx->m_resources;

                //now start new ones
                auto&& back_buffer = resources->back_buffer(device_resources::swap_chains::background);

                auto width = ctx->m_back_buffer_scaled_size.m_width;
                auto height = ctx->m_back_buffer_scaled_size.m_height;

                //may be not here
                resources->swap_chain(device_resources::swap_chains::background)->set_source_size(width, height);

                graphics->transition_resource(back_buffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
                //graphics->transition_resource(ctx->m_view_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

                graphics->set_render_target(back_buffer, ctx->m_view_depth_buffer);
                graphics->clear(back_buffer);
            }

            void render_world::end_render(render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics)
            {
                auto resources      = ctx->m_resources;
                auto back_buffer    = resources->back_buffer(device_resources::swap_chains::background);

                //Per pass
                //graphics->transition_resource(ctx->m_view_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
                graphics->transition_resource(back_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            }

            void render_world::begin_render_depth(render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics)
            {
                //graphics->transition_resource(ctx->m_view_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
                graphics->set_render_target(ctx->m_view_depth_buffer);
                graphics->clear_depth(ctx->m_view_depth_buffer, 1.0f);
            }

            void render_world::end_render_depth( render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics )
            {
                ctx;
                graphics;
                //todo: remove this,
                //graphics->transition_resource(ctx->m_view_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
            }

            void render_world::begin_render_shadows(shadow_render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics)
            {
                //graphics->transition_resource(ctx->m_shadow_depth_buffer, D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
                graphics->set_render_target(ctx->m_shadow_depth_buffer);
                graphics->clear_depth(ctx->m_shadow_depth_buffer, 1.0f);
            }

            void render_world::end_render_shadows(shadow_render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics)
            {
                ctx;
                graphics;
                //todo: remove this,
                //graphics->transition_resource(ctx->m_shadow_depth_buffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
            }

            void render_world::set_view_port( const render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics)
            {
                auto width = ctx->m_back_buffer_scaled_size.m_width;
                auto height = ctx->m_back_buffer_scaled_size.m_height;

                //Per pass  -> frequency 0
                graphics->set_view_port(viewport(width, height));
                graphics->set_scissor_rectangle(scissor(width, height));
            }

            gx::dx12::managed_graphics_command_context render_world::do_render_depth(render_context* ctx)
            {
                auto resources = ctx->m_resources;
                //now start new ones
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));
                begin_render_depth(ctx, graphics.get());
                end_render_depth(ctx, graphics.get());
                return graphics;
            }

            gx::dx12::managed_graphics_command_context render_world::do_render_shadows(shadow_render_context* ctx)
            {
                auto resources = ctx->m_resources;
                //now start new ones
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                {
                    auto profile_event = uc::gx::dx12::make_profile_event(graphics.get(), L"do_render_shadows");
                    begin_render_shadows(ctx, graphics.get());
                    end_render_shadows(ctx, graphics.get());
                }
                return graphics;
            }
        }
    }
}