#include "pch.h"
#include "uc_uwp_gx_render_world_factory.h"

#include "uc_uwp_gx_render_world_1.h"
#include "uc_uwp_gx_render_world_2.h"
#include "uc_uwp_gx_render_world_3.h"
#include "uc_uwp_gx_render_world_4.h"
#include "uc_uwp_gx_render_world_5.h"
#include "uc_uwp_gx_render_world_6.h"
#include "uc_uwp_gx_render_world_7.h"
#include "uc_uwp_gx_render_world_8.h"
#include "uc_uwp_gx_render_world_9.h"

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            std::unique_ptr<render_world> render_world_factory::create_world( world_id id, initialize_context* ctx )
            {
                switch (id)
                {
                    case world_id::scene0 : return std::unique_ptr<render_world>(new render_world_1(ctx));
                    case world_id::scene1 : return std::unique_ptr<render_world>(new render_world_2(ctx));
                    case world_id::scene2 : return std::unique_ptr<render_world>(new render_world_3(ctx));
                    case world_id::scene3 : return std::unique_ptr<render_world>(new render_world_4(ctx));
                    case world_id::scene4 : return std::unique_ptr<render_world>(new render_world_5(ctx));
					case world_id::scene5 : return std::unique_ptr<render_world>(new render_world_6(ctx));
					case world_id::scene6 : return std::unique_ptr<render_world>(new render_world_7(ctx));
					case world_id::scene7 : return std::unique_ptr<render_world>(new render_world_8(ctx));
					case world_id::scene8 : return std::unique_ptr<render_world>(new render_world_9(ctx));
                    default: return nullptr;
                }
            }

         }
    }
}