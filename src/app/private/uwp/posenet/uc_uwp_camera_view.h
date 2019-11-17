#pragma once
#include <uc_dev/util/noncopyable.h>

namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            struct gpu_graphics_command_context;
        }
    }
    
    namespace uwp
    {
        namespace gxu
        {
            struct initialize_context;

            class camera_view : public util::noncopyable
            {

                public:

                camera_view(initialize_context* c);
                void render(gx::dx12::gpu_graphics_command_context* ctx);

                private:

            };
        }
    }
    
}