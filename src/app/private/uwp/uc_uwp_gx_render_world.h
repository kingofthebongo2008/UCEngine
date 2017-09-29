#pragma once

#include <memory>
#include <vector>

#include <uc_dev/mem/align.h>

#include <uc_dev/gx/dx12/dx12.h>
#include <uc_dev/util/noncopyable.h>
#include <uc_dev/gx/pinhole_camera.h>

#include "uc_uwp_gx_submitable.h"
#include "uc_uwp_gx_render_object.h"
#include "uc_uwp_gx_render_context.h"


namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            class gpu_pixel_buffer;
        }
    }

    namespace uwp
    {
        namespace gxu
        {
            struct frame_constants
            {
                math::float4x4 m_view;
                math::float4x4 m_perspective;
            };

            struct draw_constants
            {
                math::float4x4 m_world;
            };

            inline D3D12_VIEWPORT viewport(uint32_t width, uint32_t height)
            {
                D3D12_VIEWPORT v = {};
                v.Width = static_cast<float>(width);
                v.Height = static_cast<float>(height);
                v.MinDepth = 0.0f;
                v.MaxDepth = 1.0f;
                return v;
            }

            inline D3D12_RECT scissor(uint32_t width, uint32_t height)
            {
                D3D12_RECT r = {};
                r.right = static_cast<LONG>(width);
                r.bottom = static_cast<LONG>(height);
                return r;
            }

            D3D12_VIEWPORT viewport(const gx::dx12::gpu_pixel_buffer* b);
            D3D12_RECT scissor(const gx::dx12::gpu_pixel_buffer* b);

            class render_world : public util::noncopyable
            {
                public:

                render_world(initialize_context* ctx);

                virtual ~render_world();

                void update( update_context* ctx );

                std::unique_ptr< submitable >render(render_context* ctx);
                std::unique_ptr< submitable >render_depth(render_context* ctx);
                std::unique_ptr< submitable >render_shadows(shadow_render_context* ctx);

                gx::pinhole_camera* camera()
                {
                    return m_camera.get();
                }

                const gx::pinhole_camera* camera() const
                {
                    return m_camera.get();
                }

                private:

                virtual  void do_update(update_context* ctx) = 0;
                virtual  std::unique_ptr< submitable > do_render(render_context* ctx) = 0;
                virtual  std::unique_ptr< submitable > do_render_depth(render_context* ctx);
                virtual  std::unique_ptr< submitable > do_render_shadows(shadow_render_context* ctx);

                protected:

                mem::aligned_unique_ptr<gx::pinhole_camera>	m_camera = mem::make_aligned_unique_ptr<gx::pinhole_camera>();

                static void begin_render( render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics );
                static void end_render(render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);

                static void begin_render_depth(render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);
                static void end_render_depth(render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);

                static void begin_render_shadows(shadow_render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);
                static void end_render_shadows(shadow_render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);

                static void set_view_port(const render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);
                static void set_view_port(const shadow_render_context* ctx, gx::dx12::gpu_graphics_command_context* graphics);
            };
        }
    }
}

