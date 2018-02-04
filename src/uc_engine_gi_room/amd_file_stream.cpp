#include "pch.h"

#include "amd_file_stream.h"

namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
        SimpleReadFileStream::SimpleReadFileStream(const wchar_t* file_name)
        {
            HANDLE h = CreateFile2(file_name, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);

            if (h == INVALID_HANDLE_VALUE)
            {
                throw StreamReadException();
            }

            m_handle = h;

        }

        SimpleReadFileStream::~SimpleReadFileStream()
        {
            CloseHandle(m_handle);
        }

        void SimpleReadFileStream::ReadBuffer(void* buffer, size_t buffer_size)
        {
            //ReadFile(m_handle)

        }
    }
}


