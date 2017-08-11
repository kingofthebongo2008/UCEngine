#pragma once

#include <uc_public/uc_implementation.h>
#include <uc_public/graphics/pixel_buffer.h>

namespace UniqueCreator
{
    namespace Graphics
    {
        class DepthBuffer : public PixelBuffer
        {
            public:

            class Impl;

            UC_ENGINE_API ~DepthBuffer();

            UC_ENGINE_API DepthBuffer(DepthBuffer&&) = default;
            UC_ENGINE_API DepthBuffer& operator=(DepthBuffer&&);

            Impl*   GetImpl();
            const Impl*   GetImpl() const;

            protected:
            DepthBuffer();
            details::pimpl<Impl> m_impl;
        };

        //private data, todo: fix visual assist warning
        extern template details::pimpl<DepthBuffer::Impl>;
    }
}


