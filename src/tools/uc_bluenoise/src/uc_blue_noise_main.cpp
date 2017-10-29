// ucdev_include_parser.cpp : Defines the entry point for the console application.
//
#include "pch.h"

#include <cstdint>
#include <iostream>
#include <algorithm>

#include <uc_dev/os/windows/com_initializer.h>
#include <uc_dev/gx/img/img.h>


inline std::ostream& operator<<(std::ostream& s, const std::string& str)
{
    s << str.c_str();
    return s;
}

inline std::string get_environment()
{
#if defined(_X86)
    return "x86";
#endif

#if defined(_X64)
    return "x64";
#endif
}


class media_source
{
public:

    media_source(const wchar_t* file_name) : m_path(file_name)
    {

    }

    media_source(const std::wstring& file_name) : m_path(file_name)
    {

    }

    media_source(std::wstring&& file_name) : m_path(std::move(file_name))
    {

    }

    const wchar_t* get_path() const
    {
        return m_path.c_str();
    }

private:

    std::wstring m_path;
};

class media_url
{
public:

    media_url(const wchar_t* file_name) : m_file_name(file_name)
    {

    }

    media_url(const std::wstring& file_name) : m_file_name(file_name)
    {

    }

    media_url(std::wstring&& file_name) : m_file_name(std::move(file_name))
    {

    }

    const wchar_t* get_path() const
    {
        return m_file_name.c_str();
    }

private:

    std::wstring m_file_name;
};

inline media_url make_media_url(const media_source& source, const wchar_t* path)
{
    return std::move(media_url(std::move(std::wstring(source.get_path()) + std::wstring(path))));
}


int32_t main(int32_t argc, const char* argv[])
{
    try
    {
        std::cout << "Command line:" << std::endl;
        for (auto i = 0; i < argc; ++i)
        {
            std::cout << argv[i] << " ";
        }
        std::cout << std::endl;

        uc::os::windows::com_initializer com;
        media_source source(L"../src/data//");

        const wchar_t* bluenoise_types[]  =
        {
            L"HDR_L_",
            L"HDR_LA_",
            L"HDR_RGB_",
            L"HDR_RGBA_",
            L"LDR_LLL1_",
            L"LDR_RG01_",
            L"LDR_RGB1_",
            L"LDR_RGBA_"
        };


        std::vector< uc::gx::imaging::cpu_texture> images;

        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_1.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_2.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_3.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_4.png").get_path()));

        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_5.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_6.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_7.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_8.png").get_path()));

        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_9.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_10.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_11.png").get_path()));
        images.push_back(uc::gx::imaging::read_image(make_media_url(source, L"HDR_L_12.png").get_path()));


        auto composite = uc::gx::imaging::make_image( 8 * 64, ((images.size() + 7) / 8) * 64, images[0].type());

        auto composite_pixels           = composite.pixels();
        auto composite_lock_initial     = composite_pixels.get_pixels_cpu();

        auto composite_row = 0;
        for (auto imgc = 0U; imgc < images.size(); imgc += 8)
        {
            auto composite_lock = composite_lock_initial + (composite_row++) * 64 * composite.row_pitch();

            for (auto i = 0; i < 64; ++i)
            {
                auto composite_row = composite_lock + i * composite.row_pitch();
                auto destination = composite_row;

                for (auto j = imgc; j < std::min(static_cast<size_t>(imgc + 8), images.size() ); ++j)
                {
                    auto&&      img = images[j];
                    auto img_pixels = img.pixels();
                    auto img_lock = img_pixels.get_pixels_cpu();
                    auto row = img_lock + i * img.row_pitch();
                    memcpy_s(destination, img.row_pitch(), row, img.row_pitch());
                    destination += img.row_pitch();
                }
            }
        }

        uc::gx::imaging::write_image(composite, L"test.png");
    }
    
    catch (const std::exception& e)
    {
        std::string  input_model = "uc_dev_bluenoise_r.exe";
        std::cerr << input_model << '(' << 0 << ',' << 0 << ')' << ":error 12345: missing " << e.what() << "\r\n";
        return -1;
    }

    return 0;
}

