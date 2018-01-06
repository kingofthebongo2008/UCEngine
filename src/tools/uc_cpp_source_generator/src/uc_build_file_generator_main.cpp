// ucdev_include_parser.cpp : Defines the entry point for the console application.
//
#include "pch.h"

#include <algorithm>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <experimental/filesystem>

#include "uc_build_file_generator_command_line.h"
#include <uc_dev/mem/alloc.h>

inline std::string get_environment()
{
#if defined(_X86)
    return "x86";
#endif

#if defined(_X64)
    return "x64";
#endif
}

static bool is_root_signature(const std::experimental::filesystem::path& p)
{
    return 	boost::algorithm::ends_with(p.filename().string(), "_signature.hlsl");
}

static bool is_vertex_shader(const std::experimental::filesystem::path& p)
{
    return 	boost::algorithm::ends_with(p.filename().string(), "_vertex.hlsl");
}

static bool is_pixel_shader(const std::experimental::filesystem::path& p)
{
    return 	boost::algorithm::ends_with(p.filename().string(), "_pixel.hlsl");
}

static bool is_compute_shader(const std::experimental::filesystem::path& p)
{
    return 	boost::algorithm::ends_with(p.filename().string(), "_compute.hlsl");
}

static std::string build_hlsl_tag(const std::string& shader, const std::string& shader_type)
{
    /*
    <Shader Include = "..\src\uc_dev\private\gx\dx12\shaders\static_geometry\building_vertex.hlsl">
    <ShaderPipelineStage>Vertex< / ShaderPipelineStage>
    <InProject>true< / InProject>
    </Shader>
    */
    return std::string("<Shader Include = \"") + shader + std::string("\">\r\n\t<ShaderPipelineStage>") + shader_type + std::string("</ShaderPipelineStage>\r\n") + std::string("</Shader>\r\n");
}

static void process_shaders(const std::vector< std::experimental::filesystem::path > & files)
{
    std::vector< std::experimental::filesystem::path > signatures;
    std::vector< std::experimental::filesystem::path > vertex;
    std::vector< std::experimental::filesystem::path > pixel;
    std::vector< std::experimental::filesystem::path > compute;

    for (auto&& p : files)
    {
        if (is_root_signature(p))
        {
            signatures.push_back(p);
        }
        else if (is_vertex_shader(p))
        {
            vertex.push_back(p);
        }
        else if (is_pixel_shader(p))
        {
            pixel.push_back(p);
        }
        else if (is_compute_shader(p))
        {
            compute.push_back(p);
        }
    }

    std::sort(signatures.begin(), signatures.end());
    std::sort(vertex.begin(), vertex.end());
    std::sort(pixel.begin(), pixel.end());
    std::sort(compute.begin(), compute.end());

    std::vector<std::string> lines;

    for (auto&& s : signatures)
    {
        lines.push_back(build_hlsl_tag(s.string(), "RootSignature"));
    }

    for (auto&& s : vertex)
    {
        lines.push_back(build_hlsl_tag(s.string(), "Vertex"));
    }

    for (auto&& s : pixel)
    {
        lines.push_back(build_hlsl_tag(s.string(), "Pixel"));
    }

    for (auto&& s : compute)
    {
        lines.push_back(build_hlsl_tag(s.string(), "Compute"));
    }

    for (auto&& l : lines)
    {
        std::cout << l;
    }
}

int32_t main(int32_t argc, const char* argv[])
{
    using namespace uc::build_file_generator;

    std::string input_model_error = "uc_dev_cpp_source_generator_r.exe";

    try
    {
        auto&& om   = build_option_map(argc, argv);
        auto&& vm   = std::get<0>(om);
        auto&& desc = std::get<1>(om);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        namespace fs = std::experimental::filesystem ;

        auto input_dir  = fs::path(get_input_directory(vm, fs::current_path().string()));
        auto mode       = get_mode(vm);

        std::string extension = "";

        switch (mode)
        {
            case mode::cpp:     extension = ".cpp"; break;
            case mode::hlsl:    extension = ".hlsl"; break;
            case mode::pso:     extension = ".pso"; break;
            case mode::h:       extension = ".h"; break;
        };

        std::vector< fs::path > files;

        if ( fs::exists(input_dir) && fs::is_directory( input_dir) )
        {
            fs::recursive_directory_iterator dir(input_dir);

            for (auto&& v : dir)
            {
                fs::path file = v;
                
                if ( fs::is_regular_file(file) && file.extension() == extension)
                {
                    files.push_back(file);

                }
            }
        }

        if (!files.empty())
        {
            if ( mode == mode::hlsl )
            {
                process_shaders(files);
            }
        }
        else
        {
            //todo:
            return -1;
        }
    }

    catch (const std::exception& e)
    {
        std::cerr << input_model_error << '(' << 0 << ',' << 0 << ')' << ":error 12345: missing " << e.what() << "\r\n";
        return -1;
    }

    return 0;
}

