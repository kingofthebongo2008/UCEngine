#pragma once

#include "uc_uwp_render_graph_builder.h"
#include "uc_uwp_render_graph_pass.h"
#include "uc_uwp_render_graph.h"

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
                struct graph_builder
                {
                    graph_builder();

                    std::unique_ptr<graph>  make_graph();
                    resource_allocator*     allocator() const;

                private:

                    std::unique_ptr<resource_allocator>                         m_allocator;

                    std::vector< std::unique_ptr<pass> >                        m_passes;
                    std::vector< pass* >                                        m_graphics_passes;
                    std::vector< pass* >                                        m_compute_passes;

                    std::vector< std::vector < resource * > >                   m_pass_resources;
                    std::vector< std::vector < resource* > >                    m_pass_new;
                    std::vector< std::vector < reader > >                       m_pass_inputs;
                    std::vector< std::vector < writer > >                       m_pass_outputs;

                    friend graphics_pass* add_graphics_pass(graph_builder* b, std::unique_ptr<graphics_pass> p, std::unique_ptr<pass_resource_allocator> pa);
                };

                graphics_pass* add_graphics_pass(graph_builder* b, std::unique_ptr<graphics_pass> p, std::unique_ptr<pass_resource_allocator> pa);
            }
        }
    }
}