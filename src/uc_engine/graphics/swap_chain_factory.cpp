#include "pch.h"

#include <uc_public/graphics/device_resources.h>
#include <uc_public/uc_pimpl_impl.h>
#include <uc_public/graphics/upload_queue.h>
#include <uc_public/graphics/resource_create_context.h>

#include "device_resources_impl.h"

namespace UniqueCreator
{
    namespace Graphics
    {
        template details::pimpl<DeviceResources::Impl>;

        DeviceResources::DeviceResources( ResourceCreateContext* ctx, SwapChainResources* background ) : m_impl(ctx)
        {
            background;
        }

        DeviceResources::DeviceResources(ResourceCreateContext* ctx, SwapChainResources* background, SwapChainResources* overlay) : m_impl(ctx)
        {
            background;
            overlay;
        }

        DeviceResources::~DeviceResources() = default;
        DeviceResources& DeviceResources::operator=(DeviceResources&&) = default;

        const UploadQueue* DeviceResources::GetUploadQueue() const
        {
            return m_impl->m_upload_queue_internal.get();
        }

        UploadQueue* DeviceResources::GetUploadQueue()
        {
            return m_impl->m_upload_queue_internal.get();
        }

        const ResourceCreateContext* DeviceResources::GetResourceCreateContext() const
        {
            return m_impl->m_resource_create_context;
        }

        ResourceCreateContext* DeviceResources::GetResourceCreateContext()
        {
            return m_impl->m_resource_create_context;
        }

        void DeviceResources::WaitForGpu()
        {

        }

        void DeviceResources::Present()
        {

        }
    }
}


