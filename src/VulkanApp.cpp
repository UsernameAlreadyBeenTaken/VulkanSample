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

    if (!createInstance({}, "VulkanSample", mInstance))
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

} //namespace VulkanSample
