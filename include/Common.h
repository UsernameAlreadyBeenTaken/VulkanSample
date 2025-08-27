#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "VulkanFunctions.h"

namespace VulkanSample
{

#ifdef _WIN32
#define LIBRARY_TYPE HMODULE
#elif defined _linux
#define LIBRARY_TYPE void*
#endif

#if defined _WIN32
#define LoadFunction GetProcAddress
#elif defined __linux
#define LoadFunction dlsym
#endif

bool loadVkLibrary(LIBRARY_TYPE &vkLibrary);
bool loadFunctionFromVulkanLibrary(LIBRARY_TYPE const &vkLibrary);
bool loadGlobalLevelFunctions();

bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions);
bool isExtensionSupported(std::vector<VkExtensionProperties> const &availableExtensions, const char* const extension);
bool createInstance(std::vector<const char*> &desired_extensions, const char* const appName, VkInstance &instance);

} // namespace VulkanSample
#endif // _COMMON_H_