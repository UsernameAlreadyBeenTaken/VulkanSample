#pragma once

#include <iostream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "VulkanFunctions.h"
#include "OSspecific.h"

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

struct QueueInfo
{
    uint32_t familyIndex;
    std::vector<float> priorities;
};

inline bool operator==(const QueueInfo &lhs, const QueueInfo &rhs)
{
    return lhs.familyIndex == rhs.familyIndex && lhs.priorities == rhs.priorities;
}

struct QueueParameters
{
  VkQueue   handle;
  uint32_t  familyIndex;
};

bool loadVkLibrary(LIBRARY_TYPE &vkLibrary);
bool loadFunctionFromVulkanLibrary(LIBRARY_TYPE const &vkLibrary);
bool loadGlobalLevelFunctions();

bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions);
bool isExtensionSupported(std::vector<VkExtensionProperties> const &availableExtensions, const char* const extension);
bool createInstance(std::vector<const char*> &desiredExtensions, const char* const appName, VkInstance &instance);
bool loadInstanceLevelFunctions(VkInstance &instance, std::vector<char const *> const & enabledExtensions);
bool enumerateAvailablePhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &availableDevices);
bool checkAvailableDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<VkExtensionProperties> &availableExtensions);
bool selectQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlags desiredCapabilities, uint32_t &queueFamilyIndex);
bool selectQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, uint32_t &queueFamilyIndex);
bool loadDeviceLevelFunctions(VkDevice logicalDevice, std::vector<const char *> const &enabledExtensions);
bool createLogicalDevice(VkInstance instance, VkDevice &logicalDevice, std::vector<const char*> &desiredExtensions, VkSurfaceKHR surface,
                         QueueParameters &graphicsQueue, QueueParameters &computeQueue, QueueParameters &presentQueue);
bool createPresentationSurface(VkInstance instance, WindowParameters windowParameters, VkSurfaceKHR presentationSurface);
bool selectPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkPresentModeKHR desiredMode, 
                            VkPresentModeKHR &presentMode);
bool selectSwapchainImageFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface,
                                VkSurfaceFormatKHR desiredSurfaceFormat, VkFormat &imageFormat, VkColorSpaceKHR &imageColorSpace);
bool createSwapchain(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkDevice logicalDevice,
                      VkPresentModeKHR desiredMode, VkSurfaceFormatKHR desiredSurfaceFormat,
                      VkSurfaceTransformFlagBitsKHR desiredTransform, VkImageUsageFlags swapchainImageUsage,
                      VkExtent2D &imageSize, VkFormat &imageFormat, VkSwapchainKHR &oldSwapchain, VkSwapchainKHR &swapchain, 
                      std::vector<VkImage> &swapchainImages);

void releaseVulkanLibrary(LIBRARY_TYPE &vulkanLibrary);

} // namespace VulkanSample
