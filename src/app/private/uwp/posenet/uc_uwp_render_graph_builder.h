#pragma once

#include "uc_uwp_render_graph_resources.h"
#include <vector>
#include <memory>

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
                    resource*       make_render_target(uint32_t format, uint32_t width, uint32_t height);
                    resource*       make_depth_buffer(uint32_t format, uint32_t width, uint32_t height);
                    writer          write(resource* r);
                    reader          read(resource* r);

                    std::vector< std::unique_ptr<resource> >    m_pass_resources;
                    std::vector< reader >                       m_pass_inputs;
                    std::vector< writer >                       m_pass_outputs;
                };
            }
        }
    }
}