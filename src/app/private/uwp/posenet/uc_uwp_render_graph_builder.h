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

                    protected:

                    std::vector< std::unique_ptr<resource> >  m_resources;
                };

                struct pass_resource_allocator
                {

                    pass_resource_allocator(resource_allocator* resource_allocator);

                    writer make_render_target(uint32_t format, uint32_t width, uint32_t height);
                    writer make_depth_buffer(uint32_t format, uint32_t width, uint32_t height);
                    writer make_swap_chain(void* v);

                    writer  write(resource* r, uint64_t flags);
                    reader  read(resource* r, uint64_t flags);

                    std::vector< resource* > m_pass_resources;
                    std::vector< resource* > m_pass_new;
                    std::vector< reader >    m_pass_inputs;
                    std::vector< writer >    m_pass_outputs;

                    private:

                    resource_allocator* m_resource_allocator;

                    reader add_reader(reader r);
                    writer add_writer(writer w);

                };
            }
        }
    }
}