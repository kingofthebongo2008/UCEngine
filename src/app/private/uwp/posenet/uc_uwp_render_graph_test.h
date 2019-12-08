#pragma once
#include <uc_dev/util/noncopyable.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "uc_uwp_render_graph_pass.h"
#include "uc_uwp_render_graph_resources.h"
#include "uc_uwp_render_graph_builder.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct graph
                {
                    void execute();

                    std::vector< std::unique_ptr<pass> >                        m_passes;
                    std::vector< pass* >                                        m_graphics_passes;
                    std::vector< pass* >                                        m_compute_passes;

                    std::vector< std::vector < std::unique_ptr<resource> > >    m_pass_resources;
                    std::vector< std::vector < reader > >                       m_pass_inputs;
                    std::vector< std::vector < writer > >                       m_pass_outputs;
                };

            }
        }
    }
}
    
