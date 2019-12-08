#include "pch.h"
#include "uc_uwp_render_graph_graph_builder.h"
#include "uc_uwp_render_graph_pass.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                graph_builder::graph_builder()
                {

                }

                std::unique_ptr<graph>  graph_builder::make_graph()
                {
                    return std::make_unique<graph>();
                }

                resource_allocator* graph_builder::allocator() const
                {
                    return nullptr;
                }


                void add_graphics_pass(graph_builder* b, std::unique_ptr<graphics_pass> p, std::unique_ptr<pass_resource_allocator> pa)
                {
                    pass* pass = p.get();
                    b->m_passes.emplace_back(std::move(p));
                    b->m_graphics_passes.emplace_back(pass);

                    b->m_pass_resources.resize(b->m_pass_resources.size() + 1);
                    b->m_pass_resources[b->m_pass_resources.size() - 1] = std::move(pa->m_pass_resources);

                    b->m_pass_inputs.resize(b->m_pass_inputs.size() + 1);
                    b->m_pass_inputs[b->m_pass_inputs.size() - 1] = std::move(pa->m_pass_inputs);

                    b->m_pass_outputs.resize(b->m_pass_outputs.size() + 1);
                    b->m_pass_outputs[b->m_pass_outputs.size() - 1] = std::move(pa->m_pass_outputs);
                }
            }
        }
    }
}