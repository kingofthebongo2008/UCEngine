#pragma once
#include <uc_dev/util/noncopyable.h>
#include <cstdint>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct executor;

                struct pass
                {
                    pass() = default;
                    virtual ~pass() = default;

                    pass(const pass&) = delete;
                    pass& operator=(const pass&) = delete;

                    pass(pass&&) = default;
                    pass& operator=(pass&&) = default;

                    void execute(executor* e)
                    {
                        e;
                        on_execute(e);
                    }

                private:

                    virtual void on_execute(executor* e) = 0;
                };

                struct graphics_pass : public pass
                {
                    graphics_pass() = default;
                    virtual ~graphics_pass() = default;

                    graphics_pass(const graphics_pass&) = delete;
                    graphics_pass& operator=(const graphics_pass&) = delete;

                    graphics_pass(graphics_pass&&) = default;
                    graphics_pass& operator=(graphics_pass&&) = default;

                private:
                };

                struct compute_pass : public pass
                {
                    compute_pass() = default;
                    virtual ~compute_pass() = default;

                    compute_pass(const compute_pass&) = delete;
                    compute_pass& operator=(const compute_pass&) = delete;

                    compute_pass(compute_pass&&) = default;
                    compute_pass& operator=(compute_pass&&) = default;

                private:
                };

            }
        }
    }
}
    
