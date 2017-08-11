#include "pch.h"
#include <uc_public/uc_render_factory.h>
#include "uwp/uc_uwp_renderer.h"

struct ISwapChainPanelNative;

namespace uc
{
    intrusive_ptr<irenderer> renderer_factory::create_renderer( bool* window_close, const winrt::Windows::UI::Core::CoreWindow& window, const winrt::Windows::Graphics::Display::DisplayInformation& display_information, ISwapChainPanelNative*  swapchainpanel )
    {
        return intrusive_ptr< irenderer> ( new uwp::renderer( window_close, window, display_information,  swapchainpanel ), false );
    }

    //create factory
    intrusive_ptr<renderer_factory> create_renderer_factory()
    {
        return intrusive_ptr<renderer_factory>(new renderer_factory(), false);
    }
}