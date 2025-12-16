#include "OSspecific.h"

namespace VulkanSample
{

namespace
{
  enum UserMessage
  {
    USER_MESSAGE_RESIZE = WM_USER + 1,
    USER_MESSAGE_QUIT,
    USER_MESSAGE_MOUSE_CLICK,
    USER_MESSAGE_MOUSE_MOVE,
    USER_MESSAGE_MOUSE_WHEEL
  };
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_LBUTTONDOWN:
      PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 1);
      break;
    case WM_LBUTTONUP:
      PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 0);
      break;
    case WM_RBUTTONDOWN:
      PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 1);
      break;
    case WM_RBUTTONUP:
      PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 0);
      break;
    case WM_MOUSEMOVE:
      PostMessage(hWnd, USER_MESSAGE_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam));
      break;
    case WM_MOUSEWHEEL:
      PostMessage(hWnd, USER_MESSAGE_MOUSE_WHEEL, HIWORD(wParam), 0);
      break;
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
      PostMessage(hWnd, USER_MESSAGE_RESIZE, wParam, lParam);
      break;
    case WM_KEYDOWN:
      if(VK_ESCAPE == wParam)
      {
        PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
      }
      break;
    case WM_CLOSE:
      PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
};

#ifdef VK_USE_PLATFORM_WIN32_KHR
bool createWindowHandle(WindowParameters &windowParameters, const char* title, int startX, int startY,
                        int width, int height)
{
  windowParameters.HInstance = GetModuleHandle(nullptr);

  WNDCLASSEX windowClass = {
    sizeof(WNDCLASSEX),               // UINT         cbSize
                                      /* Win 3.x */
    CS_HREDRAW | CS_VREDRAW,          // UINT         style
    WindowProcedure,                  // WNDPROC      lpfnWndProc
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