// ucdev_include_parser.cpp : Defines the entry point for the console application.
//
#pragma once

#include <vector>
#include <gsl/gsl>

#include <uc_dev/gx/lip/geo.h>


#include "uc_model_exception.h"
#include "uc_model_compressonator.h"

namespace uc
{
    namespace model
    {
        inline uc::lip::storage_format cmp_to_lip(CMP_FORMAT f)
        {
            switch (f)
            {
                case CMP_FORMAT_ARGB_8888:      return lip::storage_format::r8_g8_b8_a8_unorm;
                case CMP_FORMAT_BC1:            return lip::storage_format::bc1;
                case CMP_FORMAT_BC2:            return lip::storage_format::bc2;
                case CMP_FORMAT_BC3:            return lip::storage_format::bc3;
                case CMP_FORMAT_BC4:            return lip::storage_format::bc4;
                case CMP_FORMAT_BC5:            return lip::storage_format::bc5;
                default: throw model::exception("Invalid texture format");
            }
        }

        inline CMP_FORMAT lip_to_cmp(uc::lip::storage_format f)
        {
            switch (f)
            {
                case lip::storage_format::r8_g8_b8_a8_unorm: return CMP_FORMAT_ARGB_8888;
                case lip::storage_format::bc1:               return CMP_FORMAT_BC1;
                case lip::storage_format::bc2:               return CMP_FORMAT_BC2;
                case lip::storage_format::bc3:               return CMP_FORMAT_BC3;
                case lip::storage_format::bc4:               return CMP_FORMAT_BC4;
                case lip::storage_format::bc5:               return CMP_FORMAT_BC5;
                default: throw model::exception("Invalid texture format");
            }
        }

        inline uc::lip::storage_format image_type_to_lip(gx::imaging::image_type f)
        {
            switch (f)
            {
                case gx::imaging::image_type::r32_g32_b32_a32_float: return lip::storage_format::r32_g32_b32_a32_float;
                case gx::imaging::image_type::r16_g16_b16_a16_float: return lip::storage_format::r16_g16_b16_a16_float;
                case gx::imaging::image_type::r16_g16_b16_a16_unorm: return lip::storage_format::r16_g16_b16_a16_unorm;

                case gx::imaging::image_type::r8_g8_b8_a8_unorm: return lip::storage_format::r8_g8_b8_a8_unorm;
                case gx::imaging::image_type::b8_g8_r8_a8_unorm: return lip::storage_format::b8_g8_r8_a8_unorm;
                case gx::imaging::image_type::b8_g8_r8_x8_unorm: return lip::storage_format::b8_g8_r8_x8_unorm;

                case gx::imaging::image_type::r10_g10_b10_xr_bias_a2_unorm: return lip::storage_format::r10_g10_b10_xr_bias_a2_unorm;
                case gx::imaging::image_type::r10_g10_b10_a2_unorm: return lip::storage_format::r10_g10_b10_a2_unorm;

                case gx::imaging::image_type::b5_g5_r5_a1_unorm: return lip::storage_format::b5_g5_r5_a1_unorm;
                case gx::imaging::image_type::b5_g6_r5_unorm: return lip::storage_format::b5_g6_r5_unorm;

                case gx::imaging::image_type::r32_float: return lip::storage_format::r32_float;
                case gx::imaging::image_type::r16_float: return lip::storage_format::r16_float;
                case gx::imaging::image_type::r16_unorm: return lip::storage_format::r16_unorm;
                case gx::imaging::image_type::r8_unorm: return lip::storage_format::r8_unorm;
                case gx::imaging::image_type::a8_unorm: return lip::storage_format::a8_unorm;
                case gx::imaging::image_type::r32_g32_b32_float: return lip::storage_format::r32_g32_b32_float;

                default: throw model::exception("Invalid texture format");
            }
        }

        inline std::vector<uint8_t> convert_cmp(compressonator::texture< compressonator::texture_storage >&& source, CMP_FORMAT f)
        {
            auto p = cmp_texture(source.width(), source.height(), f);

            CMP_CompressOptions o = {};
            o.bDisableMultiThreading = TRUE;
            o.dwSize = sizeof(o);
            compressonator::throw_if_failed(CMP_ConvertTexture(&source, &std::get<0>(p), &o, nullptr, 0, 0));
            return std::get<1>(p);
        }

        inline uc::lip::texture2d create_texture_2d(const std::string& file_name)
        {
            auto r0 = gx::imaging::read_image(file_name.c_str());

            uc::lip::texture2d r;

            //storage and view formats match
            r.m_storage_format = static_cast<uint16_t>(image_type_to_lip(r0.type()));
            r.m_view_format = static_cast<uint16_t>(image_type_to_lip(r0.type()));

            auto w = r0.width();
            auto h = r0.height();

            r.m_width = static_cast<uint16_t>(w);
            r.m_height = static_cast<uint16_t>(h);

            auto span = gsl::make_span(r0.pixels().get_pixels_cpu(), r0.size());
            r.m_data.resize(span.size());
            std::copy(span.begin(), span.end(), &r.m_data[0]);

            return r;
        }

        inline uc::lip::texture2d_mip_chain create_texture_2d_mip_chain(const std::string& file_name)
        {
            auto r0 = gx::imaging::read_image(file_name.c_str());

            uc::lip::texture2d_mip_level r;

            //storage and view formats match
            r.m_storage_format = static_cast<uint16_t>(image_type_to_lip(r0.type()));
            r.m_view_format = static_cast<uint16_t>(image_type_to_lip(r0.type()));

            auto w = r0.width();
            auto h = r0.height();

            r.m_width = static_cast<uint16_t>(w);
            r.m_height = static_cast<uint16_t>(h);

            auto span = gsl::make_span(r0.pixels().get_pixels_cpu(), r0.size());
            r.m_data.resize(span.size());
            std::copy(span.begin(), span.end(), &r.m_data[0]);

            uc::lip::texture2d_mip_chain t;

            t.m_levels.push_back(std::move(r));

            return t;
        }


        namespace mip_level_computation
        {
            using image_type = gx::imaging::image_type;

            struct mip_level
            {
                uint32_t             m_width;
                uint32_t             m_height;
                std::vector<uint8_t> m_data;
            };

            struct float4
            {
                __m128 m_data;
            };

            namespace f16
            {
                inline float4 unpack_float4( uint64_t v )
                {
                    float4 r;
                    return r;
                }

                inline uint64_t pack_float4( float4 v )
                {
                    return 0;
                }

                inline uint64_t pack_float4_unorm( float4 v )
                {
                    return 0;
                }

                inline float4 unpack_float4_unorm( uint64_t v )
                {
                    float4 r;
                    return r;
                }
            }

            namespace u32
            {
                inline uint32_t pack_float4_unorm(float4 v)
                {
                    return 0;
                }

                inline float4 unpack_float4_unorm(uint32_t v)
                {
                    float4 r;
                    return r;
                }
            }

            namespace xr
            {
                inline float xr_to_float(uint32_t xr_component)
                {
                    // The & 0x3ff shows that only 10 bits contribute to the conversion. 
                    return (float)((xr_component & 0x3ff) & 0x180) / 510.f;
                }

                inline uint32_t float_to_xr(float f)
                {
                    return 0;
                }
            }

            inline int32_t clamp(int32_t x, int32_t min_value, int32_t max_value)
            {
                x = x < min_value ? min_value : x;
                x = x > max_value ? max_value : x;

                return x;
            }

            inline const void* sample_address_read(int32_t x, int32_t y, const void* img, int32_t pitch, int32_t width, int32_t height )
            {
                x = clamp(x, 0, width - 1);
                y = clamp(y, 0, height - 1);
                return reinterpret_cast<const uint8_t*>(img) + y * pitch + x;
            }

            inline void* sample_address_write(int32_t x, int32_t y, void* img, int32_t pitch, int32_t width, int32_t height)
            {
                return reinterpret_cast<uint8_t*>(img) + y * pitch + x;
            }

            template <int32_t> struct sample_image
            {
                static float4 sample(int32_t x, int32_t y, const void* img, int32_t pitch, int32_t width, int32_t height);
            };

            template <> struct sample_image<static_cast<int32_t>(image_type::r32_g32_b32_a32_float)>
            {
                static float4 load(int32_t x, int32_t y, const void* img, int32_t pitch, int32_t width, int32_t height)
                {
                    const float* address = reinterpret_cast<const float*> (sample_address_read(x, y, img, pitch, width, height));
                    float4 r;
                    r.m_data = _mm_loadu_ps(address);
                    return r;
                }

                static void store(int32_t x, int32_t y, void* img, int32_t pitch, int32_t width, int32_t height, float4 v)
                {
                    float* address = reinterpret_cast<float*> (sample_address_write(x, y, img, pitch, width, height));
                    _mm_storeu_ps(address, v.m_data);
                }
            };

            template <> struct sample_image<static_cast<int32_t>(image_type::r16_g16_b16_a16_float)>
            {
                static float4 load(int32_t x, int32_t y, const void* img, int32_t pitch, int32_t width, int32_t height)
                {
                    const int64_t* address = reinterpret_cast<const int64_t*> (sample_address_read(x, y, img, pitch, width, height));
                    __m128i        as_int = _mm_cvtsi64_si128(*address);
                    
                    float4 r;
                    r.m_data        = _mm_cvtph_ps(as_int);

                    return r;

                    //_mm_cvtsi64_si128
                    //_mm_cvtsi128_si64
                    
                    //float _cvtsh_ss(unsigned short x, int imm);
                    //unsigned short _cvtss_sh(float x, int imm);
                    //__m128 _mm_cvtph_ps(__m128i x, int imm);
                    //__m128i _mm_cvtps_ph(_m128 x, int imm);
                    
                }

                static void store(int32_t x, int32_t y, void* img, int32_t pitch, int32_t width, int32_t height, float4 v)
                {
                    int64_t* address = reinterpret_cast<int64_t*> (sample_address_write(x, y, img, pitch, width, height));
                    __m128i  as_int  = _mm_cvtps_ph(v.m_data,0);
                    int64_t  value   = _mm_cvtsi128_si64(as_int);
                    *address  = value;
                }
            };

            template <> struct sample_image<static_cast<int32_t>(image_type::r16_g16_b16_a16_unorm)>
            {
                static float4 load(int32_t x, int32_t y, const void* img, int32_t pitch, int32_t width, int32_t height)
                {
                    const int64_t* address      = reinterpret_cast<const int64_t*> (sample_address_read(x, y, img, pitch, width, height));
                    __m128i        as_int       = _mm_cvtsi64_si128(*address);
                    __m128         as_float     = _mm_cvtepi32_ps(as_int);
                    __m128         normalize    = _mm_div_ps(as_float, _mm_set_ps1(65535));

                    float4 r;
                    r.m_data = normalize;
                    return r;
                }

                static void store(int32_t x, int32_t y, void* img, int32_t pitch, int32_t width, int32_t height, float4 v)
                {
                    int64_t* address        = reinterpret_cast<int64_t*> (sample_address_write(x, y, img, pitch, width, height));
                    __m128   as_float       = _mm_mul_ps(v.m_data, _mm_set_ps1(65535));
                    __m128i  as_int         = _mm_cvtps_epi32(as_float);
                    int64_t  value          = _mm_cvtsi128_si64(as_int);

                    *address = value;
                }
            };

            template <> struct sample_image<static_cast<int32_t>(image_type::r8_g8_b8_a8_unorm)>
            {
                static float4 load(int32_t x, int32_t y, const void* img, int32_t pitch, int32_t width, int32_t height)
                {
                    //todo
                    const int32_t* address      = reinterpret_cast<const int32_t*> (sample_address_read(x, y, img, pitch, width, height));
                    __m128i        as_int       = _mm_cvtsi32_si128(*address);
                    __m128         as_float     = _mm_cvtepi32_ps(as_int);
                    __m128         normalize    = _mm_div_ps(as_float, _mm_set_ps1(255));

                    float4 r;
                    r.m_data = normalize;
                    return r;

                    //_mm_cvtsi64_si128
                    //_mm_cvtsi128_si64

                    //float _cvtsh_ss(unsigned short x, int imm);
                    //unsigned short _cvtss_sh(float x, int imm);
                    //__m128 _mm_cvtph_ps(__m128i x, int imm);
                    //__m128i _mm_cvtps_ph(_m128 x, int imm);
                }

                static void store(int32_t x, int32_t y, void* img, int32_t pitch, int32_t width, int32_t height, float4 v)
                {
                    int64_t* address = reinterpret_cast<int64_t*> (sample_address_write(x, y, img, pitch, width, height));
                    __m128   as_float = _mm_mul_ps(v.m_data, _mm_set_ps1(65535));
                    __m128i  as_int = _mm_cvtps_epi32(as_float);
                    int64_t  value = _mm_cvtsi128_si64(as_int);

                    *address = value;
                }
            };


            void test()
            {
                static float4 r = sample_image<static_cast<int32_t>(image_type::r32_g32_b32_a32_float)>::load(0, 0, nullptr, 0, 0, 0);
            }

            /*
            mip_level make_mip_level(const mip_level& p, image_type t)
            {

                auto w          = o.width();
                auto h          = o.height();
                auto pitch      = o.row_pitch();
                auto pixels_s   = reinterpret_cast<const swizzle_t*> (o.pixels().get_pixels_cpu());
                //auto size = o.size();

                auto pixels_d = reinterpret_cast<swizzle_t*> (pixels_out);

                for (auto i = 0U; i < h; ++i)
                {
                    auto rs = pixels_s + i * pitch;
                    auto ds = pixels_d + i * pitch;

                    for (auto j = 0U; j < w; ++j)
                    {
                        //rgba to 
                        swizzle_t r = *rs; rs++;
                        swizzle_t g = *rs; rs++;
                        swizzle_t b = *rs; rs++;
                        swizzle_t a = *rs; rs++;

                        //argb conversion
                        *ds = b; ds++;
                        *ds = g; ds++;
                        *ds = r; ds++;
                        *ds = a; ds++;
                    }
                }

                mip_level m;
                return m;
            }
            */
        }

        inline uc::lip::texture2d_mip_chain create_texture_2d_mip_chain(const std::string& file_name, lip::storage_format storage, lip::view_format view)
        {
            auto r0 = gx::imaging::read_image(file_name.c_str());

            uc::lip::texture2d_mip_level r;

            //only this is supported
            r.m_storage_format = static_cast<uint16_t>(storage);
            r.m_view_format = static_cast<uint16_t>(view);

            auto w = r0.width();
            auto h = r0.height();

            r.m_width = static_cast<uint16_t>(w);
            r.m_height = static_cast<uint16_t>(h);


            auto bc = convert_cmp(compressonator::make_texture(std::move(r0)), lip_to_cmp(storage));

            auto span = gsl::make_span(&bc[0], bc.size());
            r.m_data.resize(bc.size());
            std::copy(span.begin(), span.end(), &r.m_data[0]);

            uc::lip::texture2d_mip_chain t;
            t.m_levels.push_back(std::move(r));

            return t;
        }

        inline uc::lip::texture2d create_texture_2d( const std::string& file_name, lip::storage_format storage, lip::view_format view)
        {
            auto r0         = gx::imaging::read_image(file_name.c_str());

            uc::lip::texture2d r;
            //only this is supported
            r.m_storage_format = static_cast<uint16_t>(storage);
            r.m_view_format    = static_cast<uint16_t>(view);

            auto w          = r0.width();
            auto h          = r0.height();

            r.m_width       = static_cast<uint16_t>(w);
            r.m_height      = static_cast<uint16_t>(h);

            auto bc         = convert_cmp(compressonator::make_texture(std::move(r0)), lip_to_cmp(storage));

            auto span       = gsl::make_span(&bc[0], bc.size());
            r.m_data.resize(bc.size());
            std::copy(span.begin(), span.end(), &r.m_data[0]);

            return r;
        }
    }
}

