#include "pch.h"

#include <uc_dev/gx/blue_noise/moment_shadow_maps_blue_noise.h>

#include <uc_dev/gx/dx12/gpu/resource_create_context.h>
#include <uc_dev/gx/img/img.h>

namespace uc
{
    namespace gx
    {
        namespace blue_noise
        {
            struct noise_create_info
            {
                const uint8_t*    m_memory;
                size_t            m_memory_size;
            };

            static noise_create_info get_baked_noise()
            {
                #include "ldr_rg01_64x64.png.h"
                noise_create_info r = { &ldr_rg01_64x64_png[0], sizeof(ldr_rg01_64x64_png) };
                return r;
            }

            std::unique_ptr<ldr_rg01_64x64> make_blue_noise( dx12::gpu_resource_create_context*  rc, dx12::gpu_upload_queue* upload )
            {
                noise_create_info r = get_baked_noise();
                auto img            = imaging::read_image(r.m_memory, r.m_memory_size);
                auto pixels         = img.pixels().get_pixels_cpu();

                auto blue_noise     = gx::dx12::create_texture_2d_array(rc, 64, 64, DXGI_FORMAT_R8G8_UINT, 64);

                std::vector< D3D12_SUBRESOURCE_DATA > s;
                s.resize(64);

                for (auto i = 0U; i < 64; ++i)
                {
                    auto&& s0     = s[i];
                    s0.pData      = pixels + i * 64 * sizeof(uint16_t);
                    s0.RowPitch   = 64 * 64 * sizeof(uint16_t);
                    s0.SlicePitch = 64 * 64 * 64 * sizeof(uint16_t);
                }

                upload->upload_texture_2d_array(blue_noise.get(), 0, 64, &s[0]);

                return std::make_unique<ldr_rg01_64x64>(std::move(blue_noise));
            }

            ldr_rg01_64x64::ldr_rg01_64x64(dx12::managed_gpu_texture_2d_array&& textures) : m_textures( std::move(textures) )
            {

            }
        }
    }
}


