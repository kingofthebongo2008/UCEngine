#include "pch.h"
#include "uc_uwp_render_graph.h"
#include "uc_uwp_render_graph_graph_builder.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct depth_pass : graphics_pass
                {
                    depth_pass(pass_resource_allocator* b)
                    {
                        m_depth = b->make_depth_buffer(0, 0, 0);
                    }

                    resource* depth() const
                    {
                        return m_depth.m_resource;
                    }

                    private:

                    writer m_depth;

                    void on_execute(executor* ) override
                    {

                    }
                };

                struct shadow_pass : graphics_pass
                {
                    shadow_pass(pass_resource_allocator* b)
                    {
                        m_shadows = b->make_depth_buffer(0, 0, 0);
                    }

                    resource* shadows() const
                    {
                        return m_shadows.m_resource;
                    }

                    private:
                    writer  m_shadows;

                    void on_execute(executor* ) override
                    {

                    }
                };

                struct lighting_pass : graphics_pass
                {
                    lighting_pass(pass_resource_allocator* b, resource* depth, resource* shadows )
                    {
                        m_lighting  = b->make_render_target(0, 0, 0);
                        m_shadows   = b->read(shadows);
                        m_depth     = b->read(depth);
                    }

                    resource* lighting() const
                    {
                        return m_lighting.m_resource;
                    }

                private:

                    writer  m_lighting;
                    reader  m_depth;
                    reader  m_shadows;

                    void on_execute(executor* ) override
                    {

                    }
                };

                struct compose_pass : graphics_pass
                {
                    compose_pass(pass_resource_allocator* b, resource* lighting, void* swap_chain)
                    {
                        m_lighting      = b->read(lighting);
                        m_swap_chain    = b->make_swap_chain(swap_chain);
                    }

                    private:

                    reader  m_lighting;
                    writer  m_swap_chain;

                    void on_execute( executor* ) override
                    {

                    }
                };

                void Test(void* swap_chain)
                {
                    using namespace std;

                    graph_builder builder;

                    unique_ptr<pass_resource_allocator> pa_depth    = make_unique<pass_resource_allocator>(builder.allocator());
                    unique_ptr<pass_resource_allocator> pa_shadows  = make_unique<pass_resource_allocator>(builder.allocator());
                    unique_ptr<pass_resource_allocator> pa_lighting = make_unique<pass_resource_allocator>(builder.allocator());
                    unique_ptr<pass_resource_allocator> pa_compose  = make_unique<pass_resource_allocator>(builder.allocator());

                    unique_ptr<depth_pass>     depth                = make_unique<depth_pass>(pa_depth.get());
                    unique_ptr<shadow_pass>    shadows              = make_unique<shadow_pass>(pa_shadows.get());

                    unique_ptr<lighting_pass>  lighting             = make_unique<lighting_pass>(pa_lighting.get(), depth->depth(), shadows->shadows() );
                    unique_ptr<compose_pass>   compose              = make_unique<compose_pass>(pa_compose.get(), lighting->lighting(), swap_chain );

                    add_graphics_pass(&builder, std::move(depth),   std::move(pa_depth));
                    add_graphics_pass(&builder, std::move(shadows), std::move(pa_shadows));
                    add_graphics_pass(&builder, std::move(lighting), std::move(pa_lighting));
                    add_graphics_pass(&builder, std::move(compose), std::move(pa_compose));


                }
            }
        }
    }
}