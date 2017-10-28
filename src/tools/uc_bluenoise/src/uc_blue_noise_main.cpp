// ucdev_include_parser.cpp : Defines the entry point for the console application.
//
#include "pch.h"

#include <cstdint>
#include <iostream>

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
    }
    
    catch (const std::exception& e)
    {
        std::string  input_model = "uc_dev_skeleton_r.exe";
        std::cerr << input_model << '(' << 0 << ',' << 0 << ')' << ":error 12345: missing " << e.what() << "\r\n";
        return -1;
    }

    return 0;
}

