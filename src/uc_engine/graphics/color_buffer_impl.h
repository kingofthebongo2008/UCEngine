#include "pch.h"

#include <uc_public/graphics/color_buffer.h>

#include <uc_dev/gx/dx12/dx12.h>

namespace UniqueCreator
{
    namespace Graphics
    {
        class ColorBuffer::Impl
        {
            public:

            ColorBuffer::Impl()
            {

            }

            ColorBuffer::Impl(std::unique_ptr<uc::gx::dx12::gpu_color_buffer>&& b ) : m_buffer(std::move(b))
            {

            }

            uc::gx::dx12::gpu_color_buffer* buffer() const
            {
                return m_buffer.get();
            }

            private:
            std::unique_ptr< uc::gx::dx12::gpu_color_buffer > m_buffer;
        };
    }
}


