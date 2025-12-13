#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

namespace VulkanSample
{

struct WindowParameters
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
    HINSTANCE          HInstance;
    HWND               HWnd;
#elif defined VK_USE_PLATFORM_XLIB_KHR
    Display          * Dpy;
    Window             Window;
#elif defined VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t * Connection;
    xcb_window_t       Window;
#endif
};

bool createWindowHandle(WindowParameters &windowParameters, const char* title, int startX, int startY,
                        int width, int height);
void destroyWindowHandle(WindowParameters &windowParameters);

} // namespace VulkanSample