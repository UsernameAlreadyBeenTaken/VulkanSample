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

    // enumerate phys devices + check their extensions

    return true;
}

} //namespace VulkanSample
