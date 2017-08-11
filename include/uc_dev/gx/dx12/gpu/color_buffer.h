#pragma once

#include <uc_dev/gx/dx12/gpu/virtual_resource.h>
#include <uc_dev/gx/dx12/gpu/pixel_buffer.h>
#include <uc_dev/gx/dx12/gpu/descriptor_heap.h>

namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            class gpu_color_buffer : public gpu_pixel_buffer
            {

            private:

                using base = gpu_pixel_buffer;

            public:
                gpu_color_buffer(ID3D12Resource* resource, descriptor_handle rtv, descriptor_handle srv, descriptor_handle uav) : base(resource)
                    , m_RTV(rtv)
                    , m_SRV(srv)
                    , m_UAV(uav)
                {

                }

                descriptor_handle   rtv() const
                {
                    return m_RTV;
                }

                descriptor_handle   srv() const
                {
                    return m_SRV;
                }

                descriptor_handle   uav() const
                {
                    return m_UAV;
                }

            private:

                descriptor_handle   m_RTV;
                descriptor_handle   m_SRV;
                descriptor_handle   m_UAV;

            };
        }
    }
}


