#pragma once

#include <uc_dev/gx/dx12/gpu/virtual_resource.h>
#include <uc_dev/gx/dx12/gpu/pixel_buffer.h>
#include <uc_dev/gx/dx12/gpu/descriptor_heap.h>
#include <uc_dev/gx/dx12/gpu/descriptor_heap.h>

namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            class gpu_back_buffer : public gpu_pixel_buffer
            {

            private:
                using base = gpu_pixel_buffer;

            public:
                gpu_back_buffer() : base(nullptr)
                {

                }

                explicit gpu_back_buffer(ID3D12Resource* resource) : base(resource)
                {

                }

                gpu_back_buffer(ID3D12Resource* resource, descriptor_handle rtv) : base(resource)
                    , m_RTV(rtv)
                {

                }

                descriptor_handle   rtv() const
                {
                    return m_RTV;
                }

                private:

                descriptor_handle   m_RTV;
            };
        }
    }
}


