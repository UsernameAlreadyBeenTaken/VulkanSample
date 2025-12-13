#include "OSspecific.h"

namespace VulkanSample
{

#ifdef VK_USE_PLATFORM_WIN32_KHR
bool createWindowHandle(WindowParameters &windowParameters, const char* title, int startX, int startY,
                        int width, int height)
{
  windowParameters.HInstance = GetModuleHandle( nullptr );

  WNDCLASSEX windowClass = {
    sizeof(WNDCLASSEX),               // UINT         cbSize
                                      /* Win 3.x */
    CS_HREDRAW | CS_VREDRAW,          // UINT         style
    nullptr,                          // WNDPROC      lpfnWndProc
    0,                                // int          cbClsExtra
    0,                                // int          cbWndExtra
    windowParameters.HInstance,       // HINSTANCE    hInstance
    nullptr,                          // HICON        hIcon
    LoadCursor(nullptr, IDC_ARROW),   // HCURSOR      hCursor
    (HBRUSH)(COLOR_WINDOW + 1),       // HBRUSH       hbrBackground
    nullptr,                          // LPCSTR       lpszMenuName
    "VulkanSample",                   // LPCSTR       lpszClassName
                                      /* Win 4.0 */
    nullptr                           // HICON        hIconSm
  };

  if(!RegisterClassEx(&windowClass))
    return false;

  windowParameters.HWnd = CreateWindow("VulkanSample", title, WS_OVERLAPPEDWINDOW, startX, startY, width, height,
                                       nullptr, nullptr, windowParameters.HInstance, nullptr);

  if(!windowParameters.HWnd)
    return false;

  return true;
}

void destroyWindowHandle(WindowParameters &windowParameters)
{
  if(windowParameters.HWnd)
  {
      DestroyWindow(windowParameters.HWnd);
      windowParameters.HWnd = nullptr;
  }

  if(windowParameters.HInstance)
    UnregisterClass("VulkanSample", windowParameters.HInstance);
}
#else
bool createWindowHandle(WindowParameters &windowParameters, const char* title, int startX, int startY, 
                        int width, int height)
{
    return false; // not implemented yet and probably will not be ever
}

void destroyWindowHandle(WindowParameters &windowParameters)
{
    // not implemented yet and probably will not be ever
}
#endif

} // namespace VulkanSample