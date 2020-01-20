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

                struct shadow_terrain_pass : graphics_pass
                {
                    shadow_terrain_pass(pass_resource_allocator* b, resource* shadows)
                    {
                        m_shadows = b->read(shadows, flags::flags_non_pixel_shader_resource);
                    }

                    resource* shadows() const
                    {
                        return m_shadows.m_resource;
                    }

                private:
                    reader  m_shadows;

                    void on_execute(executor*) override
                    {

                    }
                };

                struct shadow_terrain_pass2 : graphics_pass
                {
                    shadow_terrain_pass2(pass_resource_allocator* b, resource* shadows)
                    {
                        m_shadows = b->write(shadows, flags::flags_depth_write);
                    }

                    resource* shadows() const
                    {
                        return m_shadows.m_resource;
                    }

                private:
                    writer  m_shadows;

                    void on_execute(executor*) override
                    {

                    }
                };

                struct direct_lighting_pass : graphics_pass
                {
                    direct_lighting_pass(pass_resource_allocator* b, resource* depth, resource* shadows )
                    {
                        m_lighting  = b->make_render_target(0, 0, 0);
                        m_shadows   = b->read(shadows, flags::flags_non_pixel_shader_resource);
                        m_depth     = b->read(depth, flags::flags_non_pixel_shader_resource);
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

                struct ambient_occlusion_pass : graphics_pass
                {
                    ambient_occlusion_pass(pass_resource_allocator* b, resource* depth, resource* direct_lighting)
                    {
                        m_lighting = b->make_render_target(0, 0, 0);
                        m_direct_lighting = b->read(direct_lighting, flags::flags_non_pixel_shader_resource);
                        m_depth  = b->read(depth, flags::flags_non_pixel_shader_resource | flags::flags_pixel_shader_resource);
                    }

                    resource* lighting() const
                    {
                        return m_lighting.m_resource;
                    }

                private:

                    writer  m_lighting;
                    reader  m_depth;
                    reader  m_direct_lighting;

                    void on_execute(executor*) override
                    {

                    }
                };

                struct compose_pass : graphics_pass
                {
                    compose_pass(pass_resource_allocator* b, resource* direct, resource* indirect, void* swap_chain)
                    {
                        m_direct        = b->read(direct, flags::flags_non_pixel_shader_resource);
                        m_indirect      = b->read(indirect, flags::flags_non_pixel_shader_resource);
                        m_swap_chain    = b->make_swap_chain(swap_chain);
                    }

                    private:

                    reader  m_direct;
                    reader  m_indirect;
                    writer  m_swap_chain;

                    void on_execute( executor* ) override
                    {

                    }
                };

                template <typename t, typename ...args > t* add_graphic_pass(graph_builder* b, args&& ... a)
                {
                    using namespace std;
                    unique_ptr<pass_resource_allocator> pass_allocator = make_unique<pass_resource_allocator>(b->allocator());
                    unique_ptr<t>                       pass           = make_unique<t>(pass_allocator.get(), forward<args>(a)...);
                    return static_cast<t*> (add_graphics_pass(b, std::move(pass), std::move(pass_allocator)));
                }

                void Test(void* swap_chain)
                {
                    using namespace std;

                    graph_builder builder;

                    depth_pass*                 depth               = add_graphic_pass<depth_pass>(&builder);
                    shadow_pass*                shadows             = add_graphic_pass<shadow_pass>(&builder);
                    shadow_terrain_pass*        shadows_terrain     = add_graphic_pass<shadow_terrain_pass>(&builder,  shadows->shadows());
                    shadow_terrain_pass2* shadows_terrain2          = add_graphic_pass<shadow_terrain_pass2>(&builder, shadows->shadows());
                    direct_lighting_pass*  direct                   = add_graphic_pass<direct_lighting_pass>(&builder, depth->depth(), shadows->shadows() );
                    ambient_occlusion_pass* indirect                = add_graphic_pass<ambient_occlusion_pass>(&builder, depth->depth(), direct->lighting());
                    compose_pass*   compose                         = add_graphic_pass<compose_pass>(&builder, direct->lighting(), indirect->lighting(), swap_chain );

                    compose;
                    shadows_terrain;
                    shadows_terrain2;

                    std::unique_ptr<graph> g = builder.make_graph();
                    execution_pass plan      = g->execute();
                }
            }
        }
    }
}