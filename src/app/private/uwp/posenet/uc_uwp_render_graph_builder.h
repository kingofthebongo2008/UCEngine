#pragma once

#include "uc_uwp_render_graph_resources.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct builder
                {
                    render_target   make_render_target(uint32_t format, uint32_t width, uint32_t height);
                    depth_buffer    make_depth_buffer(uint32_t format, uint32_t width, uint32_t height);
                    writer          write(resource* r);
                    reader          read(resource* r);
                };
            }
        }
    }
}