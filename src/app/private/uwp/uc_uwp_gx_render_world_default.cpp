#include "pch.h"
#include "uc_uwp_gx_render_world_default.h"

#include <gsl/gsl>
#include <ppl.h>

#include "uc_uwp_device_resources.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            render_world_default::render_world_default(initialize_context* c) : base(c)
            {
            }

            render_world_default::~render_world_default()
            {

            }

            void render_world_default::do_update(update_context*)
            {

            }

            gx::dx12::managed_graphics_command_context render_world_default::do_render(render_context* ctx)
            {
                //now start new ones
                auto resources = ctx->m_resources;
                auto graphics = create_graphics_command_context(resources->direct_command_context_allocator(device_resources::swap_chains::background));

                begin_render(ctx, graphics.get());
                end_render(ctx, graphics.get());

                return graphics;
            }

            gx::dx12::managed_graphics_command_context render_world_default::do_render_depth( render_context* )
            {
                return gx::dx12::managed_graphics_command_context();
            }
        }
    }
}