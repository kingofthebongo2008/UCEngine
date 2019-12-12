#pragma once
#include <uc_dev/util/noncopyable.h>
#include <cstdint>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct resource
                {
                    resource() = default;
                    virtual ~resource() = default;

                    resource(const resource&) = delete;
                    resource& operator=(const resource&) = delete;

                    resource(resource&&) = default;
                    resource& operator=(resource&&) = default;
                };

                struct render_target  final : public resource
                {

                };

                struct depth_buffer final : public resource
                {

                };

                struct swap_chain final : public resource
                {
                    swap_chain(void* e) : m_external_resource(e) {}

                    void* m_external_resource;
                };

                struct writer
                {
                    resource* m_resource;
                };

                struct reader
                {
                    resource* m_resource;
                    uint64_t* m_flags;
                };
            }
        }
    }
}
    
