#pragma once


#include <uc_dev/gx/dx12/cmd/graphics_compute_command_context.h>
#include <uc_dev/gx/dx12/cmd/command_context_utils.h>


namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            using managed_graphics_compute_command_context = std::unique_ptr< gpu_graphics_compute_command_context, details::gpu_command_context_deleter >;

            template <typename ...args>
            inline managed_graphics_compute_command_context create_graphics_compute_command_context(gpu_command_context_allocator* rc, args&&... a)
            {
                std::unique_ptr<gpu_base_command_context> base(rc->create_base_command_context(std::forward<args>(a)...));
                return managed_graphics_command_context(static_cast< gpu_graphics_compute_command_context* > ( base.release() ), details::gpu_command_context_deleter(rc));
            }

        }
    }
}