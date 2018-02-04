#pragma once

#include <exception>
#include "amd_base_types.h"

namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
        class StreamReadException : public std::exception
        {

        };

        class SimpleReadFileStream
        {

        public:

            SimpleReadFileStream(const wchar_t* file_name);
            ~SimpleReadFileStream();
            void ReadBuffer(void* buffer, size_t buffer_size);

        private:

            SimpleReadFileStream(const SimpleReadFileStream &) = delete;
            SimpleReadFileStream& operator=(const SimpleReadFileStream &) = delete;

            HANDLE m_handle;
        };

    }
}

