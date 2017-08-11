#pragma once

#include <cstdint>
#include <uc_public/uc_irenderer.h>
#include <uc_public/uc_intrusive_ptr.h>

#include <uc_public/uc_render_defines.h>


#include "winrt/base.h"
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

struct ISwapChainPanelNative;

namespace uc
{
    //responsible for creating renderer
    class renderer_factory : public referenced_object
    {
        public:

        virtual intrusive_ptr<irenderer> create_renderer( bool* window_close, const winrt::Windows::UI::Core::CoreWindow& window, const winrt::Windows::Graphics::Display::DisplayInformation& display_information, ISwapChainPanelNative*  swapchainpanel = nullptr );
    };

    //create factory
    intrusive_ptr<renderer_factory> UC_RENDER_DLL create_renderer_factory();
}
