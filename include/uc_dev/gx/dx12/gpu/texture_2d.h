#pragma once

#include <uc_dev/gx/dx12/gpu/virtual_resource.h>
#include <uc_dev/gx/dx12/gpu/descriptor_heap.h>

namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            class gpu_texture_2d : public gpu_virtual_resource
            {
                private:

                    using base = gpu_virtual_resource;

                public:

                    gpu_texture_2d( ) : base( nullptr )
                    {

                    }

                    gpu_texture_2d(ID3D12Resource* resource, persistent_gpu_srv_descriptor_heap_handle uav, persistent_gpu_srv_descriptor_heap_handle srv ) : 
                        base(resource)
                        , m_uav(uav)
                        , m_srv(srv)
                    {

                    }

                    ~gpu_texture_2d()
                    {
                        if (m_uav.m_heap)
                        {
                            m_uav.m_heap->free(m_uav.m_handle);
                        }

                        if (m_srv.m_heap)
                        {
                            m_srv.m_heap->free(m_srv.m_handle);
                        }
                    }

                    descriptor_handle uav() const
                    {
                        return m_uav.m_handle;
                    }

                    descriptor_handle srv() const
                    {
                        return m_srv.m_handle;
                    }

                private:

                    persistent_gpu_srv_descriptor_heap_handle    m_uav;
                    persistent_gpu_srv_descriptor_heap_handle    m_srv;

            };
        }
    }
}
