#pragma once

#include <uc_public/uc_referenced_object.h>

#include <ppltasks.h>
#include <memory>

#include <winrt/base.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Xaml.Controls.h>


namespace uc
{
    class irenderer : public referenced_object
    {
        public:

        virtual ~irenderer() {}
    
        virtual concurrency::task<void> initialize_async() = 0;
        virtual concurrency::task<void> shutdown_async() = 0;

        virtual void update() = 0;
        virtual void render() = 0;
        virtual void present() = 0;
        virtual void resize() = 0;

        virtual void set_window(const winrt::Windows::UI::Core::CoreWindow& window) = 0;
        virtual void set_display_info(const winrt::Windows::Graphics::Display::DisplayInformation& display_information) = 0;
        virtual void set_swapchainpanel(const winrt::Windows::UI::Xaml::Controls::SwapChainPanel&  swapchainpanel) = 0;

        virtual void refresh_display_layout() = 0;
    };
}
