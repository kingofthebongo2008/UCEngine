#pragma once

#include "uc_uwp_render_graph_resources.h"
#include <vector>
#include <memory>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct resource_allocator
                {

                    resource* make_render_target(uint32_t format, uint32_t width, uint32_t height);
                    resource* make_depth_buffer(uint32_t format, uint32_t width, uint32_t height);
                    resource* make_swap_chain(void* v);

                    private:

                    std::vector< std::unique_ptr<resource> >  m_pass_resources;
                };

                struct pass_resource_allocator
                {

                    pass_resource_allocator(resource_allocator* resource_allocator);

                    writer make_render_target(uint32_t format, uint32_t width, uint32_t height);
                    writer make_depth_buffer(uint32_t format, uint32_t width, uint32_t height);
                    writer make_swap_chain(void* v);

                    writer  write(resource* r);
                    reader  read(resource* r);

                    std::vector< resource* > m_pass_resources;
                    std::vector< reader >    m_pass_inputs;
                    std::vector< writer >    m_pass_outputs;

                    private:

                    resource_allocator* m_resource_allocator;

                    resource* make_render_target_(uint32_t format, uint32_t width, uint32_t height);
                    resource* make_depth_buffer_(uint32_t format, uint32_t width, uint32_t height);
                    resource* make_swap_chain_(void* v);
                };
            }
        }
    }
}