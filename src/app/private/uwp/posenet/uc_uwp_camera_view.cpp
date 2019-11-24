#include "pch.h"
#include "uc_uwp_camera_view.h"

#include "../uc_uwp_gx_render_context.h"
#include "../uc_uwp_device_resources.h"

#include <uc_dev/gx/dx12/cmd/profiler.h>
#include <uc_dev/gx/dx12/dx12.h>

#include <autogen/shaders/camera_view_graphics.h>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct executor
                {
                    void begin_command_list() {}
                    void end_command_list() {}

                    void begin_execute() {}
                    void begin_end() {}
                };
            }

            camera_view::camera_view(initialize_context* resources)
            {
                m_pso = gx::dx12::create_pso(resources->m_resources->device_d2d12(), resources->m_resources->resource_create_context(), gx::dx12::camera_view_graphics::create_pso);
            }

            void camera_view::render(gx::dx12::gpu_graphics_command_context* graphics)
            {
                auto profile_event = gx::dx12::make_profile_event(graphics, L"Camera View");
                D3D12_VERTEX_BUFFER_VIEW v = {};
                D3D12_INDEX_BUFFER_VIEW  i = {};
                graphics->set_vertex_buffer(0, v);
                graphics->set_index_buffer(i);
                graphics->set_pso(m_pso);
                graphics->draw(4);
            }
        }
    }
}