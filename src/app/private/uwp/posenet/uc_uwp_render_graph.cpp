#include "pch.h"
#include "uc_uwp_render_graph.h"
#include "uc_uwp_render_graph_builder.h"

#include <vector>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                void graph::execute()
                {
                    execution_pass execution;

                    if (!m_passes.empty())
                    {
                        //obtain indices of the used passes, remove all passes not used
                        std::vector<uint32_t> payload;
                        payload.reserve(m_passes.size());

                        for (auto p = 0; p < m_passes.size(); ++p)
                        {
                            bool break_pass = false;
                            for (auto out = 0; out < m_pass_resources[p].size(); ++out)
                            {
                                resource* r = m_pass_resources[p][out];
                                bool b = false;

                                if (r->is_external())
                                {
                                    payload.push_back(p);
                                    break_pass = true;
                                    break;
                                }
                                else
                                {
                                    for (auto pn = p + 1; pn < m_passes.size(); ++pn)
                                    {
                                        for (auto in = 0; in < m_pass_resources[pn].size(); ++in)
                                        {
                                            resource* rn = m_pass_resources[pn][in];

                                            if (rn == r)
                                            {
                                                payload.push_back(p);
                                                b = true;
                                                break;
                                            }
                                        }

                                        if (b)
                                        {
                                            break_pass = true;
                                            break;
                                        }
                                    }

                                    if (break_pass)
                                    {
                                        break;
                                    }
                                }
                            }
                        }

                        for (int32_t i = 0; i < payload.size(); ++i)
                        {

                            //transitions -> read  -> write
                            //transitions -> write -> read

                            int32_t pi = payload[i];

                            std::vector<execution_pass::transitions_read>       trr;
                            std::vector<execution_pass::transitions_write>      trw;
                            std::vector<execution_pass::transitions_create>     trc;
                            std::vector<execution_pass::transitions_destroy>    trd;

                            //scan inputs
                            for (auto inp = 0; inp < m_pass_inputs[pi].size(); ++inp)
                            {
                                reader r = m_pass_inputs[pi][inp];
                                bool b = false;

                                //all previous passes
                                for (int32_t pp = i - 1; pp >= 0; pp--)
                                {
                                    uint32_t pl = payload[pp];

                                    //previous passes outputs
                                    for (auto out = 0; out < m_pass_outputs[pl].size(); ++out)
                                    {
                                        writer w = m_pass_outputs[pl][out];

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

                            //allocate resources
                            //scan resources // transition to write if we need
                            for (auto out = 0; out < m_pass_new[pi].size(); ++out)
                            {
                                //create new resources, they are not supposed to be nor in the inputs, nor in the outputs
                                trc.push_back({ m_pass_new[pi][out] });
                            }

                            //scan outputs
                            for (auto out = 0; out < m_pass_outputs[pi].size(); ++out)
                            {
                                writer w = m_pass_outputs[pi][out];
                                bool b = false;

                                //all previous passes
                                for (int32_t pp = i - 1; pp >= 0; pp--)
                                {
                                    uint32_t pl = payload[pp];

                                    for (auto inp = 0; inp < m_pass_inputs[pl].size(); ++inp)
                                    {
                                        reader r = m_pass_inputs[pl][inp];

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

                            //Now try to do garbage collect
                            for (auto out = 0; out < m_pass_resources[pi].size(); ++out)
                            {
                                resource* res = m_pass_resources[pi][out];
                                bool b = false;

                                //scan outputs
                                for (auto pn = i + 1; pn < payload.size(); ++pn)
                                {
                                    uint32_t pl = payload[pn];

                                    for (auto w = 0; w < m_pass_resources[pl].size(); ++w)
                                    {
                                        resource* rn = m_pass_resources[pl][w];

                                        if (rn == res)
                                        {
                                            b = true;
                                            break;
                                        }
                                    }

                                    if (b)
                                    {
                                        break;
                                    }
                                }

                                //If the resource is found nowhere, delete it
                                if (b == false && !res->is_external() )
                                {
                                    trd.push_back({ res });
                                }
                            }

                            pass* c = m_passes[pi].get();

                            execution.m_passes.push_back(c);
                            execution.m_trr.emplace_back(trr);
                            execution.m_trw.emplace_back(trw);
                            execution.m_trc.emplace_back(trc);
                            execution.m_trd.emplace_back(trd);
                        }
                    }
                }
            }
        }
    }
}