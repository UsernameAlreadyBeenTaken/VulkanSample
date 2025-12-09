#include "VulkanApp.h"

namespace VulkanSample
{

bool VulkanApp::init()
{
    if (!loadVkLibrary(mVkLibrary))
        return false;

    if (!loadFunctionFromVulkanLibrary(mVkLibrary))
        return false;

    if(!loadGlobalLevelFunctions())
        return false;

    std::vector<const char*> desiredExtensions;
    desiredExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    desiredExtensions.emplace_back(
#ifdef VK_USE_PLATFORM_WIN32_KHR
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined VK_USE_PLATFORM_XCB_KHR
      VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined VK_USE_PLATFORM_XLIB_KHR
      VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
    );

    if (!createInstance(desiredExtensions, "VulkanSample", mInstance))
        return false;

    if (!loadInstanceLevelFunctions(mInstance, {}))
        return false;

    std::vector<VkPhysicalDevice> physicalDevices;
    if (!enumerateAvailablePhysicalDevices(mInstance, physicalDevices))
        return false;

    for (auto &physicalDevice : physicalDevices)
    {
        
    }

    return true;
}

VulkanApp::~VulkanApp()
{
    destroyVulkanObjects(mLogicalDevice, mInstance);
    releaseVulkanLibrary(mVkLibrary);
}

} //namespace VulkanSample
