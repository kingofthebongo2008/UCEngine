#pragma once

#include <uc_public/graphics/depth_buffer.h>
#include <uc_dev/gx/dx12/dx12.h>

namespace UniqueCreator
{
    namespace Graphics
    {
        class DepthBufferInternal : public DepthBuffer
        {
            public:

            DepthBufferInternal( std::unique_ptr<uc::gx::dx12::gpu_depth_buffer>&& b );
        };
    }
}


