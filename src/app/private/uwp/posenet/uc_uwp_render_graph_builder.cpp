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
                    std::unique_ptr< resource > r = std::make_unique<render_target>();
                    resource* target = r.get();

                    format;
                    width;
                    height;

                    m_resources.emplace_back(std::move(r));
                    return target;
                }

                resource* resource_allocator::make_depth_buffer(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;

                    std::unique_ptr< resource > r = std::make_unique<depth_buffer>();
                    resource* target = r.get();

                    m_resources.emplace_back(std::move(r));
                    return target;
                }

                resource* resource_allocator::make_swap_chain(void* v)
                {
                    std::unique_ptr< resource > r = std::make_unique<swap_chain>( v );
                    resource* target = r.get();

                    m_resources.emplace_back(std::move(r));
                    return target;
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////
                pass_resource_allocator::pass_resource_allocator(resource_allocator* resource_allocator) : m_resource_allocator(resource_allocator)
                {

                }

                writer pass_resource_allocator::make_render_target(uint32_t format, uint32_t width, uint32_t height)
                {
                    resource* r = m_resource_allocator->make_render_target(format, width, height);
                    writer w = { r };
                    m_pass_resources.push_back(r);
                    m_pass_new.push_back(r);
                    m_pass_outputs.push_back(w);
                    return w;
                }

                writer pass_resource_allocator::make_depth_buffer(uint32_t format, uint32_t width, uint32_t height)
                {
                    resource* r = m_resource_allocator->make_depth_buffer(format, width, height);
                    writer w = { r };
                    m_pass_resources.push_back(r);
                    m_pass_new.push_back(r);
                    m_pass_outputs.push_back(w);
                    return w;
                }

                writer pass_resource_allocator::make_swap_chain(void* v)
                {
                    resource* r = m_resource_allocator->make_swap_chain(v);
                    writer w    = { r };
                    m_pass_resources.push_back(r);
                    m_pass_outputs.push_back(w);
                    return w;
                }

                writer pass_resource_allocator::write(resource* r)
                {
                    writer w = { r };
                    m_pass_resources.push_back(r);
                    m_pass_outputs.push_back(w);
                    return w;
                }

                reader pass_resource_allocator::read(resource* r)
                {
                    reader w = { r };
                    m_pass_resources.push_back(r);
                    m_pass_inputs.push_back(w);
                    return w;
                }
            }
        }
    }
}