#include "VulkanApp.h"

namespace VulkanSample
{

VulkanApp::VulkanApp()
{
    mInstance      = VK_NULL_HANDLE;
    mSurface       = VK_NULL_HANDLE;
    mLogicalDevice = VK_NULL_HANDLE;
}

bool VulkanApp::init(WindowParameters windowParameters)
{
    if (!loadVkLibrary(mVkLibrary))
        return false;

    if (!loadFunctionFromVulkanLibrary(mVkLibrary))
        return false;

    if(!loadGlobalLevelFunctions())
        return false;

    std::vector<const char*> desiredInstanceExtensions;
    desiredInstanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    desiredInstanceExtensions.emplace_back(
#ifdef VK_USE_PLATFORM_WIN32_KHR
      VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined VK_USE_PLATFORM_XCB_KHR
      VK_KHR_XCB_SURFACE_EXTENSION_NAME
#elif defined VK_USE_PLATFORM_XLIB_KHR
      VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#endif
    );

    if (!createInstance(desiredInstanceExtensions, "VulkanSample", mInstance))
        return false;

    if (!loadInstanceLevelFunctions(mInstance, {}))
        return false;

    if(!createPresentationSurface(mInstance, windowParameters, mSurface))
        return false;

    std::vector<VkPhysicalDevice> physicalDevices;
    if (!enumerateAvailablePhysicalDevices(mInstance, physicalDevices))
        return false;

    std::vector<const char*> desiredDeviceExtensions;
    desiredDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    if(!createLogicalDevice(mInstance, mLogicalDevice, desiredDeviceExtensions, mSurface, mGraphicsQueue, mComputeQueue, mPresentQueue))
        return false;

    return true;
}

VulkanApp::~VulkanApp()
{
  if(mLogicalDevice)
    vkDestroyDevice(mLogicalDevice, nullptr);

  if(mSurface)
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

  if(mInstance)
    vkDestroyInstance(mInstance, nullptr);

  releaseVulkanLibrary(mVkLibrary);
}

} //namespace VulkanSample
