#pragma once

#include <cstdint>
#include <memory>
#include <tuple>


#include <uc_dev/util/utf8_conv.h>

#include <uc_dev/gx/img/cpu_imaging.h>
#include <uc_dev/gx/img/cpu_imaging_utils_base.h>
#include <uc_dev/gx/img/cpu_imaging_utils_cpu.h>

namespace uc
{
    namespace gx
    {
        namespace imaging
        {
            using cpu_texture =  texture < cpu_texture_storage >;

            inline image_type wic_to_image_type(const WICPixelFormatGUID& guid)
            {
                struct WICTranslate
                {
                    GUID                wic;
                    image_type          format;
                };
                
                const WICTranslate g_WICFormats[] =
                {
                    { GUID_WICPixelFormat128bppRGBAFloat,       image_type::r32_g32_b32_a32_float },

                    { GUID_WICPixelFormat64bppRGBAHalf,         image_type::r16_g16_b16_a16_float },
                    { GUID_WICPixelFormat64bppRGBA,             image_type::r16_g16_b16_a16_unorm },

                    { GUID_WICPixelFormat32bppRGBA,             image_type::r8_g8_b8_a8_unorm },
                    { GUID_WICPixelFormat32bppBGRA,             image_type::b8_g8_r8_a8_unorm }, // DXGI 1.1
                    { GUID_WICPixelFormat32bppBGR,              image_type::b8_g8_r8_x8_unorm }, // DXGI 1.1

                    { GUID_WICPixelFormat32bppRGBA1010102XR,    image_type::r10_g10_b10_xr_bias_a2_unorm }, // DXGI 1.1
                    { GUID_WICPixelFormat32bppRGBA1010102,      image_type::r10_g10_b10_a2_unorm },

                    { GUID_WICPixelFormat16bppBGRA5551,         image_type::b5_g5_r5_a1_unorm },
                    { GUID_WICPixelFormat16bppBGR565,           image_type::b5_g6_r5_unorm },

                    { GUID_WICPixelFormat32bppGrayFloat,        image_type::r32_float },
                    { GUID_WICPixelFormat16bppGrayHalf,         image_type::r16_float },
                    { GUID_WICPixelFormat16bppGray,             image_type::r16_unorm },
                    { GUID_WICPixelFormat8bppGray,              image_type::r8_unorm },

                    { GUID_WICPixelFormat8bppAlpha,             image_type::a8_unorm },
                    { GUID_WICPixelFormat96bppRGBFloat,         image_type::r32_g32_b32_float },
                };

                for (size_t i = 0; i < _countof(g_WICFormats); ++i)
                {
                    if (g_WICFormats[i].wic == guid)
                    {
                        return g_WICFormats[i].format;
                    }
                }

                return image_type::unknown;
            }

            inline WICPixelFormatGUID wic_to_wic(const WICPixelFormatGUID& guid)
            {
                struct WICConvert
                {
                    GUID        source;
                    GUID        target;
                };

                const WICConvert g_WICConvert[] =
                {
                    // Note target GUID in this conversion table must be one of those directly supported formats (above).
                    { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

                    { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

                    { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
                    { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

                    { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
                    { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

                    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

                    { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

                    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

                    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

                    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                    { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                    { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                    { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                    { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                    { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

                    { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
                    { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
                    { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
                    { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
                    { GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

                    { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
                    { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

                    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
                    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
                    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
                };

                for (size_t i = 0; i < _countof(g_WICConvert); ++i)
                {
                    if (g_WICConvert[i].source == guid)
                    {
                        return g_WICConvert[i].target;
                    }
                }

                return GUID_WICPixelFormat32bppRGBA;
            }


            inline cpu_texture read_image(const wchar_t* url_path)
            {
                auto factory    = imaging::create_factory();
                auto stream0    = imaging::create_stream_reading(factory, url_path);
                auto decoder0   = imaging::create_decoder_reading(factory, stream0);
                auto frame0     = imaging::create_decode_frame(decoder0);

                imaging::bitmap_source bitmap(frame0);

                auto format     = bitmap.get_pixel_format();
                auto size       = bitmap.get_size();

                auto bpp        = imaging::wic_bits_per_pixel(factory, bitmap.get_pixel_format() );
                auto row_pitch  = (bpp * std::get<0>(size) + 7) / 8;
                auto row_height = std::get<1>(size);
                auto image_size = row_pitch * row_height;

                WICPixelFormatGUID pixel_format;
                throw_if_failed(frame0->GetPixelFormat(&pixel_format));

                WICPixelFormatGUID convert_pixel_format = pixel_format;


                std::unique_ptr<uint8_t[]> temp;

                auto type = wic_to_image_type(pixel_format);

                if (type == image_type::unknown )
                {
                    convert_pixel_format = wic_to_wic(pixel_format);

                    type = wic_to_image_type(convert_pixel_format);

                    if (type == image_type::unknown)
                    {
                        throw_if_failed(E_FAIL);
                    }
                }

                if (convert_pixel_format != pixel_format)
                {
                    auto scaler = create_bitmap_scaler(factory, frame0, std::get<0>(size), std::get<1>(size));

                    if ( false)
                    {
                        bpp = imaging::wic_bits_per_pixel(factory, convert_pixel_format);
                        row_pitch = (bpp * std::get<0>(size) + 7) / 8;
                        row_height = std::get<1>(size);
                        image_size = row_pitch * row_height;
                        std::unique_ptr<uint8_t[]> temp2(new (std::nothrow) uint8_t[image_size]);
                        throw_if_failed(scaler->CopyPixels(nullptr, static_cast<uint32_t>(row_pitch), static_cast<uint32_t>(image_size), temp2.get()));
                        temp = std::move(temp2);

                    }
                    else
                    {
                        bpp = imaging::wic_bits_per_pixel(factory, convert_pixel_format);
                        row_pitch = (bpp * std::get<0>(size) + 7) / 8;
                        row_height = std::get<1>(size);
                        image_size = row_pitch * row_height;


                        auto converter = create_format_converter(factory, scaler, pixel_format, convert_pixel_format);
                        std::unique_ptr<uint8_t[]> temp2(new (std::nothrow) uint8_t[image_size]);
                        throw_if_failed(converter->CopyPixels(nullptr, static_cast<uint32_t>(row_pitch), static_cast<uint32_t>(image_size), temp2.get()));
                        temp = std::move(temp2);
                    }
                }
                else
                {
                    std::unique_ptr<uint8_t[]> temp2 (new (std::nothrow) uint8_t[image_size]);
                    bitmap.copy_pixels(nullptr, static_cast<uint32_t>(row_pitch), static_cast<uint32_t>(image_size), temp2.get());
                    temp = std::move(temp2);
                }

                return cpu_texture(std::get<0>(size), std::get<1>(size), type, temp.release());
            }

            inline cpu_texture read_image(const char* url_path)
            {
                auto u = util::utf16_from_utf8(url_path);
                return read_image(u.c_str());
            }

            template <typename texture > inline void write_image(const texture& t, const wchar_t* url_path)
            {
                using namespace os::windows;

                auto factory = imaging::create_factory();
                auto stream0 = imaging::create_stream_writing(factory, url_path);
                auto encoder0 = imaging::create_encoder_writing(factory, stream0);
                auto frame0 = imaging::create_encode_frame(encoder0);

                throw_if_failed(frame0->SetSize(t.get_width(), t.get_height()));

                WICPixelFormatGUID formatGUID;
                WICPixelFormatGUID formatGUID_required;

                switch (t.get_image_type())
                {
                    case rgb:
                    {
                        formatGUID = formatGUID_required = GUID_WICPixelFormat24bppBGR;
                    }
                    break;

                    case grayscale:
                    {
                        formatGUID = formatGUID_required = GUID_WICPixelFormat8bppGray;
                    }
                    break;

                    case float32:
                    {
                        formatGUID = formatGUID_required = GUID_WICPixelFormat32bppGrayFloat;
                    }
                    break;
                }

                throw_if_failed(frame0->SetPixelFormat(&formatGUID));
                throw_if_failed(IsEqualGUID(formatGUID, formatGUID_required));

                auto proxy = t.get_pixels();

                throw_if_failed(frame0->WritePixels(t.get_height(), t.get_pitch(), t.get_size(), proxy.get_pixels_cpu()));
                throw_if_failed(frame0->Commit());
                throw_if_failed(encoder0->Commit());
            }
        }
    }
}
