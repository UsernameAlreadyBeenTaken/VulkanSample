#pragma once

#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
#define LIBRARY_TYPE HMODULE
#elif defined _linux
#define LIBRARY_TYPE void*
#endif

namespace VulkanSample
{

bool loadVkLibrary(LIBRARY_TYPE& vkLibrary);

} // namespace VulkanSample