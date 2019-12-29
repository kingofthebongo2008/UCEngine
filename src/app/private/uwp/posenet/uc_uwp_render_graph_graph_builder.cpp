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
                graph_builder::graph_builder() : m_allocator ( std::make_unique< resource_allocator  > ())
                {

                }

                resource_allocator* graph_builder::allocator() const
                {
                    return m_allocator.get();
                }

                graphics_pass* add_graphics_pass(graph_builder* b, std::unique_ptr<graphics_pass> p, std::unique_ptr<pass_resource_allocator> pa)
                {
                    graphics_pass* pass = p.get();
                    b->m_passes.emplace_back(std::move(p));
                    b->m_graphics_passes.emplace_back(pass);

                    b->m_pass_new.resize(b->m_pass_new.size() + 1);
                    if (!pa->m_pass_new.empty())
                    {
                        b->m_pass_new[b->m_pass_new.size() - 1] = std::move(pa->m_pass_new);
                    }

                    b->m_pass_resources.resize(b->m_pass_resources.size() + 1);
                    if (!pa->m_pass_resources.empty())
                    {
                        b->m_pass_resources[b->m_pass_resources.size() - 1] = std::move(pa->m_pass_resources);
                    }

                    b->m_pass_inputs.resize(b->m_pass_inputs.size() + 1);
                    if (!pa->m_pass_inputs.empty())
                    {
                        b->m_pass_inputs[b->m_pass_inputs.size() - 1] = std::move(pa->m_pass_inputs);
                    }

                    b->m_pass_outputs.resize(b->m_pass_outputs.size() + 1);
                    if (!pa->m_pass_outputs.empty())
                    {
                        b->m_pass_outputs[b->m_pass_outputs.size() - 1] = std::move(pa->m_pass_outputs);
                    }

                    return pass;
                }


                std::unique_ptr<graph> graph_builder::make_graph()
                {
                    std::unique_ptr<graph> g = std::make_unique<graph>();

                    g->m_allocator = std::move(m_allocator);
                    g->m_compute_passes = std::move(m_compute_passes);
                    g->m_graphics_passes = std::move(m_graphics_passes);
                    g->m_passes = std::move(m_passes);
                    g->m_pass_new = std::move(m_pass_new);
                    g->m_pass_inputs = std::move(m_pass_inputs);
                    g->m_pass_outputs = std::move(m_pass_outputs);
                    g->m_pass_resources = std::move(m_pass_resources);

                    return g;
                }
            }
        }
    }
}