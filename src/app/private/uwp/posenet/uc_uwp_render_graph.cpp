#include "pch.h"
#include "uc_uwp_render_graph.h"
#include "uc_uwp_render_graph_builder.h"

#include <vector>
#include <unordered_map>
#include <map>
#include <assert.h>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                namespace
                {
                    struct transitions_read_write
                    {
                        reader m_r;
                        writer m_w;
                    };

                    struct transitions_write_read
                    {
                        writer  m_w;
                        reader  m_r;
                    };

                    struct transitions_read_read
                    {
                        reader  m_r0;
                        reader  m_r1;
                    };

                    struct transitions_write_write
                    {
                        writer  m_w0;
                        writer  m_w1;
                    };

                    inline bool operator==(const execution_pass::transition& o0, const execution_pass::transition& o1)
                    {
                        return o0.m_before == o1.m_before && o0.m_after == o1.m_after;
                    }

                    struct execution_device_simulator
                    {
                        uint64_t after_state(transitions_read_read r)
                        {
                            return r.m_r0.m_flags | r.m_r1.m_flags;
                        }

                        uint64_t after_state(transitions_read_write r)
                        {
                            return r.m_w.m_flags;
                        }

                        uint64_t after_state(transitions_write_write r)
                        {
                            return r.m_w1.m_flags;
                        }

                        uint64_t after_state(transitions_write_read r)
                        {
                            return r.m_r.m_flags;
                        }

                        bool set_state(resource* r, execution_pass::transition t, execution_pass::transition_resource* result)
                        {
                            auto&& r0 = m_states.find(r);

                            assert(t.m_after != t.m_before);

                            if (r0 == m_states.end())
                            {
                                m_states[r] = t;
                                result->m_resource = r;
                                result->m_transition = t;
                                return true;
                            }
                            else
                            {
                                if (r0->second == t)
                                {
                                    assert(false);
                                    return false;
                                }
                                else
                                {
                                    assert(r0->second.m_after == t.m_before);
                                    m_states[r] = t;
                                    result->m_resource = r;
                                    result->m_transition = t;
                                    return true;
                                }
                            }
                        }

                        bool set_state(transitions_read_read r, execution_pass::transition_resource* result)
                        {
                            uint64_t before = r.m_r0.m_flags;
                            uint64_t after  = after_state(r);
                            auto     t = execution_pass::transition{ before, after };
                            return set_state(r.m_r0.m_resource, t, result);
                        }

                        
                        bool set_state(transitions_read_write r, execution_pass::transition_resource* result)
                        {
                            uint64_t before = r.m_r.m_flags;
                            uint64_t after = after_state(r);
                            auto     t = execution_pass::transition{ before, after };
                            return set_state(r.m_r.m_resource, t, result);
                        }

                        bool set_state(transitions_write_read r, execution_pass::transition_resource* result)
                        {
                            uint64_t before = r.m_w.m_flags;
                            uint64_t after = after_state(r);
                            auto     t = execution_pass::transition{ before, after };
                            return set_state(r.m_r.m_resource, t, result);
                        }

                        bool set_state(transitions_write_write r, execution_pass::transition_resource* result)
                        {
                            uint64_t before = r.m_w0.m_flags;
                            uint64_t after = after_state(r);
                            auto     t = execution_pass::transition{ before, after };
                            return set_state(r.m_w0.m_resource, t, result);
                        }
                        

                        std::unordered_map<resource*, execution_pass::transition > m_states;
                    };
                }

                execution_pass graph::execute()
                {
                    execution_pass execution;

                    std::vector<std::vector<transitions_read_read>>              trr_;
                    std::vector<std::vector<transitions_read_write>>             trw_;
                    std::vector<std::vector<transitions_write_read>>             twr_;
                    std::vector<std::vector<transitions_write_write>>            tww_;

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

                            std::vector<transitions_read_read>       trr;
                            std::vector<transitions_read_write>      trw;
                            std::vector<transitions_write_read>      twr;
                            std::vector<transitions_write_write>     tww;

                            std::vector<execution_pass::transitions_create>          trc;
                            std::vector<execution_pass::transitions_destroy>         trd;

                            //scan inputs
                            for (auto inp = 0; inp < m_pass_inputs[pi].size(); ++inp)
                            {
                                reader r = m_pass_inputs[pi][inp];
                                bool b   = false;

                                int32_t  min_read_index  = -1;
                                int32_t  min_write_index = -1;

                                reader r1;
                                writer w1;

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
                                            min_write_index = pp;
                                            w1              = w;
                                            b               = true;
                                            break;
                                        }
                                    }

                                    if (b) break;
                                }

                                //all previous passes
                                {
                                    b = false;

                                    for (int32_t pp = i - 1; pp >= 0; pp--)
                                    {
                                        uint32_t pl = payload[pp];

                                        //previous passes inputs
                                        for (auto out = 0; out < m_pass_inputs[pl].size(); ++out)
                                        {
                                            reader w = m_pass_inputs[pl][out];

                                            if (r.m_resource == w.m_resource)
                                            {
                                                min_read_index = pp;
                                                r1 = w;
                                                b = true;
                                                break;
                                            }
                                        }

                                        if (b) break;
                                    }
                                }

                                //read resource not found previously. assume it is a common state
                                if (min_read_index == -1 && min_write_index == -1)
                                {
                                    //twr.push_back({ r1, r } );
                                    //assert(false);
                                }
                                else if (min_read_index == -1 && min_write_index != -1)
                                {
                                    assert(w1.m_flags != r.m_flags);
                                    twr.push_back({ w1, r });
                                }
                                else if (min_read_index != -1 && min_write_index == -1)
                                {
                                    if (r1.m_flags != r.m_flags)
                                    {
                                        trr.push_back({ r1, r });
                                    }
                                }
                                else
                                {
                                    if (min_read_index > min_write_index)
                                    {
                                        if (r1.m_flags != r.m_flags)
                                        {
                                            trr.push_back({ r1, r });
                                        }
                                    }
                                    else
                                    {
                                        assert(min_read_index != min_write_index); //a resource cannot be input and output in the same pass
                                        assert(w1.m_flags != r.m_flags);
                                        twr.push_back({ w1,r });
                                    }
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
                                writer w    = m_pass_outputs[pi][out];
                                bool b      = false;

                                int32_t  min_read_index = -1;
                                int32_t  min_write_index = -1;

                                reader r1;
                                writer w1;

                                //all previous passes
                                for (int32_t pp = i - 1; pp >= 0; pp--)
                                {
                                    uint32_t pl = payload[pp];

                                    for (auto inp = 0; inp < m_pass_inputs[pl].size(); ++inp)
                                    {
                                        reader r = m_pass_inputs[pl][inp];

                                        if (r.m_resource == w.m_resource)
                                        {
                                            r1 = r;
                                            min_read_index = pp;
                                            b = true;
                                            break;
                                        }
                                    }

                                    if (b) break;
                                }

                                {
                                    b = false;

                                    //all previous passes
                                    for (int32_t pp = i - 1; pp >= 0; pp--)
                                    {
                                        uint32_t pl = payload[pp];

                                        for (auto inp = 0; inp < m_pass_outputs[pl].size(); ++inp)
                                        {
                                            writer r = m_pass_outputs[pl][inp];

                                            if (r.m_resource == w.m_resource && r.m_flags != w.m_flags )
                                            {
                                                w1 = r;
                                                min_write_index = pp;
                                                b = true;
                                                break;
                                            }
                                        }
                                        if (b) break;
                                    }
                                }

                                //
                                if (min_read_index == -1 && min_write_index == -1)
                                {
                                    //valid, new resources or imported external resources start with write
                                }
                                else if (min_read_index == -1 && min_write_index != -1)
                                {
                                    assert(w1.m_flags != w.m_flags);
                                    tww.push_back({ w1, w });
                                }
                                else if (min_read_index != -1 && min_write_index == -1)
                                {
                                    assert(r1.m_flags != w.m_flags);
                                    trw.push_back({ r1, w });
                                }
                                else
                                {
                                    if (min_read_index > min_write_index)
                                    {
                                        assert(r1.m_flags != w.m_flags);
                                        trw.push_back({ r1, w });
                                    }
                                    else
                                    {
                                        assert(min_read_index != min_write_index); //a resource cannot be input and output in the same pass
                                        assert(w1.m_flags != w.m_flags);
                                        tww.push_back({ w1, w });
                                    }
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
                            trr_.emplace_back(trr);
                            trw_.emplace_back(trw);
                            twr_.emplace_back(twr);
                            tww_.emplace_back(tww);
                            execution.m_trc.emplace_back(trc);
                            execution.m_trd.emplace_back(trd);
                            
                        }

                        execution.m_tr.resize(execution.m_passes.size());

                        //simulate execution and filter redundant states
                        execution_device_simulator simulator;
                        for (auto i = 0; i < execution.m_passes.size(); ++i)
                        {
                            std::vector<execution_pass::transition_resource>                 tr;


                            for (auto&& r : trr_[i])
                            {
                                execution_pass::transition_resource t;
                                if (simulator.set_state(r, &t) )
                                {
                                    tr.push_back(t);
                                }
                            }

                            for (auto&& r : trw_[i])
                            {
                                execution_pass::transition_resource t;
                                if (simulator.set_state(r, &t))
                                {
                                    tr.push_back(t);
                                }
                            }

                            for (auto&& r : twr_[i])
                            {
                                execution_pass::transition_resource t;
                                if (simulator.set_state(r, &t))
                                {
                                    tr.push_back(t);
                                }
                            }

                            for (auto&& r : tww_[i])
                            {
                                execution_pass::transition_resource t;
                                if (simulator.set_state(r, &t))
                                {
                                    tr.push_back(t);
                                }
                            }

                            execution.m_tr[i] = std::move(tr);
                        }
                    }

                    return execution;
                }
            }
        }
    }
}