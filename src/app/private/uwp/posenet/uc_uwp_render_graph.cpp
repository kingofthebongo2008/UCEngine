#include "pch.h"
#include "uc_uwp_render_graph.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                void graph::add_graphics_pass(std::unique_ptr<graphics_pass> p, std::unique_ptr<builder> b)
                {
                    pass* pass = p.get();
                    m_passes.emplace_back(std::move(p));
                    m_graphics_passes.emplace_back(pass);

                    m_pass_resources.resize(m_pass_resources.size() + 1);
                    m_pass_resources[m_pass_resources.size() - 1] = std::move(b->m_pass_resources);

                    m_pass_inputs.resize(m_pass_inputs.size() + 1);
                    m_pass_inputs[m_pass_inputs.size() - 1] = std::move(b->m_pass_inputs);

                    m_pass_outputs.resize(m_pass_outputs.size() + 1);
                    m_pass_outputs[m_pass_outputs.size() - 1] = std::move(b->m_pass_outputs);
                }

                void graph::add_compute_pass(std::unique_ptr<compute_pass> p, std::unique_ptr<builder> b)
                {
                    pass* pass = p.get();
                    m_passes.emplace_back(std::move(p));
                    m_compute_passes.emplace_back(pass);

                    m_pass_resources.resize(m_pass_resources.size() + 1);
                    m_pass_resources[m_pass_resources.size() - 1] = std::move(b->m_pass_resources);

                    m_pass_inputs.resize(m_pass_inputs.size() + 1);
                    m_pass_inputs[m_pass_inputs.size() - 1] = std::move(b->m_pass_inputs);

                    m_pass_outputs.resize(m_pass_outputs.size() + 1);
                    m_pass_outputs[m_pass_outputs.size() - 1] = std::move(b->m_pass_outputs);
                }

                void graph::import_resource()
                {

                }

                void graph::execute()
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

                    if (!m_passes.empty())
                    {
                        for (auto i = 0U; i < m_passes.size(); ++i)
                        {
                            //transitions -> read  -> write
                            //transitions -> write -> read

                            std::vector<transitions_read>  trr;
                            std::vector<transitions_write> trw;

                            pass* c     = m_passes[i].get();
w                            
                            //scan inputs
                            for (auto inp = 0; inp < m_pass_inputs[i].size();++i)
                            {
                                reader r    = m_pass_inputs[i][inp];
                                bool b      = false;

                                //all passes
                                for (auto p = i-1; p >=0; p--)
                                {
                                    //previous passes outputs
                                    for (auto out = 0; out < m_pass_outputs[p].size(); ++out)
                                    {
                                        writer w = m_pass_outputs[p][out];

                                        if (r.m_resource == w.m_resource)
                                        {
                                           //side effect
                                            trr.push_back({ r,w });
                                            b = true;
                                            break;
                                        }
                                    }

                                    if (b) break;
                                }
                            }
                            
                            //scan outputs
                            for (auto out = 0; out < m_pass_outputs[i].size(); ++i)
                            {
                                writer w = m_pass_outputs[i][out];
                                bool b = false;

                                //all passes
                                for (auto p = -1; p >= 0; p--)
                                {
                                    for (auto inp = 0; inp < m_pass_inputs[p].size(); ++inp)
                                    {
                                        reader r = m_pass_inputs[p][inp];

                                        if (r.m_resource == w.m_resource)
                                        {
                                            //side effect
                                            trw.push_back({ w,r });
                                            b = true;
                                            break;
                                        }
                                    }
                                    if (b) break;
                                }
                            }
                            
                            //allocate resources
                            //scan resources // transition to write if we need
                            for (auto out = 0; out < m_pass_resources[i].size(); ++i)
                            {
                                
                            }

                            //execute transitions
                            //note: new output resources are always in write initial state
                            //every pass is in a separate command list, so we can benefit from promotion and decay
                            c->execute(nullptr);
                        }
                    }
                }
            }
        }
    }
}