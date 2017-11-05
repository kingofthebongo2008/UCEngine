#pragma once

#include <memory>
#include <uc_dev/util/noncopyable.h>
#include <uc_dev/gx/dx12/dx12.h>


namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            class gpu_resource_create_context;
            class gpu_upload_queue;
            class gpu_texture_2d_array;
        }

        namespace blue_noise
        {
            class ldr_rg01_64x64 : private util::noncopyable
            {

                public:
                ldr_rg01_64x64(dx12::managed_gpu_texture_2d_array&& textures);

                ldr_rg01_64x64(ldr_rg01_64x64&& o)              = default;
                ldr_rg01_64x64& operator= (ldr_rg01_64x64&&o)   = default;

                const dx12::gpu_texture_2d_array* texture_array_2d() const
                {
                    return m_textures.get();
                }

                dx12::gpu_texture_2d_array* texture_array_2d()
                {
                    return m_textures.get();
                }

                private:

                dx12::managed_gpu_texture_2d_array m_textures;
            };

            std::unique_ptr<ldr_rg01_64x64> make_blue_noise(dx12::gpu_resource_create_context*  rc, dx12::gpu_upload_queue* upload );
        }
    }
}
