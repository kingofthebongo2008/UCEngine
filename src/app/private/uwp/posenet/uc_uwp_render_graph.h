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
                    struct transitions_create
                    {
                        resource* m_r;                   
                    };

                    struct transitions_destroy
                    {
                        resource* m_r;
                    };

                    struct transition
                    {
                        uint64_t    m_before = 0;
                        uint64_t    m_after  = 0;
                    };

                    struct transition_resource
                    {
                        transition m_transition;
                        resource*  m_resource = nullptr;
                    };

                    std::vector< pass* >                                        m_passes;

                    std::vector<std::vector<transition_resource>>               m_tr;
                    std::vector<std::vector<transitions_create>>                m_trc;  //resources that we need to create
                    std::vector<std::vector<transitions_destroy>>               m_trd;  //resources that we need to destroy
                };

                struct graph
                {
                    execution_pass execute();

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
    
