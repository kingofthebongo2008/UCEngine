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
                struct execution_pass
                {
                    struct transitions_read
                    {
                        reader m_r;
                        writer m_w;
                    };

                    struct transitions_write
                    {
                        writer m_w;
                        reader m_r;
                    };

                    struct transitions_create
                    {
                        resource* m_r;
                    };

                    struct transitions_destroy
                    {
                        resource* m_r;
                    };

                    std::vector< pass* >                                        m_passes;

                    std::vector<std::vector<transitions_read>>                  m_trr;  //resources that we need to read
                    std::vector<std::vector<transitions_write>>                 m_trw;  //resources that we need to write
                    std::vector<std::vector<transitions_create>>                m_trc;  //resources that we need to create
                    std::vector<std::vector<transitions_destroy>>               m_trd;  //resources that we need to destroy
                };

                struct graph
                {
                    void execute();

                    std::unique_ptr<resource_allocator>                         m_allocator;

                    std::vector< std::unique_ptr<pass> >                        m_passes;
                    std::vector< pass* >                                        m_graphics_passes;
                    std::vector< pass* >                                        m_compute_passes;

                    std::vector< std::vector < resource* > >                    m_pass_resources;
                    std::vector< std::vector < resource* > >                    m_pass_new;
                    std::vector< std::vector < reader > >                       m_pass_inputs;
                    std::vector< std::vector < writer > >                       m_pass_outputs;
                };

            }
        }
    }
}
    
