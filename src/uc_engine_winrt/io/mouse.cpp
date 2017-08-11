#include "pch.h"

#include "mouse.h"

#include <windows.ui.core.h>
#include <wrl/client.h>
#include <winrt/base.h>

namespace UniqueCreatorEngine
{
	namespace WinRT
	{
		namespace InputOutput
		{
			inline auto MakeMouseParams(Windows::UI::Core::ICoreWindow ^w, float dpi)
			{
				::UniqueCreator::InputOutput::CreateMouseParams r;

				auto w0 = reinterpret_cast<winrt::abi<winrt::Windows::UI::Core::ICoreWindow>*>(w);
				winrt::Windows::UI::Core::CoreWindow w1(nullptr);
				winrt::attach(w1, w0);

				r.m_window	= w1;
				r.m_dpi		= dpi;

				return r;
			}

			Mouse::Mouse(Windows::UI::Core::CoreWindow ^w, float dpi) : m_native_mouse(MakeMouseParams(w,dpi))
			{

			}

			MouseState Mouse::Sample()
			{
				auto s = m_native_mouse.Sample();
				
				MouseState r;

				r.Buttons	= s.m_Buttons;
				r.X			= s.m_X;
				r.Y			= s.m_Y;
					
				return r;
			}
		}
	}
}

