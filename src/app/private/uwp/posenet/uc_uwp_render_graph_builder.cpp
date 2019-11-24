#include "pch.h"
#include "uc_uwp_render_graph_builder.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                render_target builder::make_render_target(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;
                    return {};
                }

                depth_buffer builder::make_depth_buffer(uint32_t format, uint32_t width, uint32_t height)
                {
                    format;
                    width;
                    height;
                    return {};
                }

                writer builder::write(resource* r)
                {
                    r;
                    return {};
                }

                reader  builder::read(resource* r)
                {
                    r;
                    return {};
                }
            }
        }
    }
}