#include "pch.h"
#include "uc_uwp_render_graph_builder.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                resource* resource_allocator::make_render_target(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;
                    return nullptr;
                }

                resource* resource_allocator::make_depth_buffer(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;
                    return nullptr;
                }

                resource* resource_allocator::make_swap_chain(void* v)
                {
                    v;
                    return nullptr;
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////
                pass_resource_allocator::pass_resource_allocator(resource_allocator* resource_allocator) : m_resource_allocator(resource_allocator)
                {

                }

                writer pass_resource_allocator::make_render_target(uint32_t format, uint32_t width, uint32_t height)
                {
                    return writer{ m_resource_allocator->make_render_target(format, width, height) };
                }

                writer pass_resource_allocator::make_depth_buffer(uint32_t format, uint32_t width, uint32_t height)
                {
                    return writer{ m_resource_allocator->make_depth_buffer(format, width, height) };
                }

                writer pass_resource_allocator::make_swap_chain(void* v)
                {
                    return writer{ m_resource_allocator->make_swap_chain(v) };
                }

                writer pass_resource_allocator::write(resource* r)
                {
                    r;
                    return {  };
                }

                reader pass_resource_allocator::read(resource* r)
                {
                    r;
                    return {  };
                }

                resource* pass_resource_allocator::make_render_target_(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;
                    return {  };
                }

                resource* pass_resource_allocator::make_depth_buffer_(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;
                    return {  };
                }

                resource* pass_resource_allocator::make_swap_chain_(void* v)
                {
                    v;
                    return {  };
                }
            }
        }
    }
}