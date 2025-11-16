#pragma once

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
bool createInstance(std::vector<const char*> &desiredExtensions, const char* const appName, VkInstance &instance);
bool loadInstanceLevelFunctions(VkInstance &instance, std::vector<char const *> const & enabledExtensions);
bool enumerateAvailablePhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &availableDevices);
bool checkAvailableDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<VkExtensionProperties> &availableExtensions);
void getFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures &deviceFeatures,
                                              VkPhysicalDeviceProperties & deviceProperties);

} // namespace VulkanSample
