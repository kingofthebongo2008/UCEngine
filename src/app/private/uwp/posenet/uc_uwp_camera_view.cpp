#include "pch.h"
#include "uc_uwp_camera_view.h"

#include "../uc_uwp_gx_render_context.h"
#include "../uc_uwp_device_resources.h"


#include <uc_dev/gx/dx12/dx12.h>

#include <autogen/shaders/camera_view_graphics.h>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            camera_view::camera_view(initialize_context* resources)
            {
                m_pso = gx::dx12::create_pso(resources->m_resources->device_d2d12(), resources->m_resources->resource_create_context(), gx::dx12::camera_view_graphics::create_pso);

            }

            void camera_view::render(gx::dx12::gpu_graphics_command_context* )
            {


            }
        }
    }
}