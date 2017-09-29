#pragma once

#include <memory>
#include <vector>

#include <uc_dev/gx/dx12/dx12.h>
#include <uc_dev/util/noncopyable.h>

namespace uc
{
    namespace uwp
    {
        class submitable
        {
            public:
            submitable() = default;
            virtual ~submitable() = default;

            void submit()
            {
                on_submit();
            }

            private:
                    
            submitable(const submitable&) = delete;
            submitable& operator=(const submitable&) = delete;

            virtual void on_submit() = 0;
        };

        class graphics_submitable : public submitable
        {

            public:

            graphics_submitable(gx::dx12::managed_graphics_command_context&& ctx) : m_ctx(std::move(ctx))
            {

            }

            private:
            graphics_submitable(const graphics_submitable&) = delete;
            graphics_submitable& operator=(const submitable&) = delete;

            gx::dx12::managed_graphics_command_context m_ctx;

            void on_submit()
            {
                m_ctx->submit();
            }
        };
    }
}

