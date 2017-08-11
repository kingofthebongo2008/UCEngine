#pragma once

#include <uc_dev/gx/dx12/gpu/pipeline_state.h>
#include <uc_dev/gx/dx12/cmd/command_context.h>

namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            struct gpu_copy_command_context : public gpu_command_context
            {
                using base = gpu_command_context;
            };
        }
    }
}