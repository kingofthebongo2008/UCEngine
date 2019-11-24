#pragma once
#include <uc_dev/util/noncopyable.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "uc_uwp_render_graph_pass.h"
#include "uc_uwp_render_graph_resources.h"

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
                    void add_graphics_pass(std::unique_ptr<graphics_pass> p)
                    {
                        pass* pass = p.get();
                        m_passes.emplace_back(std::move(p));
                        m_graphics_passes.emplace_back(pass);
                    }

                    void add_compute_pass(std::unique_ptr<compute_pass> p)
                    {
                        pass* pass = p.get();
                        m_passes.emplace_back(std::move(p));
                        m_compute_passes.emplace_back(pass);
                    }

                    void import_resource()
                    {

                    }

                    std::vector< std::unique_ptr<pass> >        m_passes;
                    std::vector< pass* >                        m_graphics_passes;
                    std::vector< pass* >                        m_compute_passes;

                    std::vector< resource* >                    m_pass_resources;
                    std::vector< reader >                       m_pass_inputs;
                    std::vector< writer >                       m_pass_outputs;
                    std::vector < std::unique_ptr<resource> >   m_resources;
                };

            }
        }
    }
}
    
