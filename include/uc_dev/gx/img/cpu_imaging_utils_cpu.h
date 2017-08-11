#pragma once

#include <cstdint>
#include <memory>

namespace uc
{
    namespace gx
    {
        namespace imaging
        {
            class cpu_texture_storage
            {

            public:

                class storage_proxy
                {

                public:

                    storage_proxy(uint8_t* pixels) : m_pixels(pixels)
                    {

                    }

                    uint8_t* get_pixels_cpu() const
                    {
                        return m_pixels;
                    }

                private:

                    uint8_t* m_pixels;
                };

                cpu_texture_storage( uint8_t pixels[], size_t ) :
                    m_pixels(pixels)
                {

                }

                cpu_texture_storage(cpu_texture_storage&& o ) :
                    m_pixels(std::move(o.m_pixels))
                {

                }

                cpu_texture_storage& operator=(cpu_texture_storage&& o)
                {
                    m_pixels = std::move(o.m_pixels);
                    return *this;
                }

                storage_proxy  pixels() const
                {
                    return storage_proxy(m_pixels.get());
                }

            private:
                cpu_texture_storage(const cpu_texture_storage&);
                cpu_texture_storage& operator=(const cpu_texture_storage&);
                std::unique_ptr< uint8_t[] > m_pixels;


            };
        }
    }
}
