#include "pch.h"

#include <uc_public/graphics/depth_buffer.h>

#include <uc_dev/gx/dx12/dx12.h>

namespace UniqueCreator
{
    namespace Graphics
    {
        class DepthBuffer::Impl
        {
            public:

            DepthBuffer::Impl()
            {

            }

            DepthBuffer::Impl(std::unique_ptr<uc::gx::dx12::gpu_depth_buffer>&& b ) : m_buffer(std::move(b))
            {

            }

            uc::gx::dx12::gpu_depth_buffer* buffer() const
            {
                return m_buffer.get();
            }

            private:
            std::unique_ptr< uc::gx::dx12::gpu_depth_buffer > m_buffer;
        };
    }
}


