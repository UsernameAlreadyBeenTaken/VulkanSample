#include "VulkanApp.h"

namespace VulkanSample
{

bool VulkanApp::init()
{
    if (!loadVkLibrary(vkLibrary))
        return false;

    if (!loadFunctionFromVulkanLibrary(vkLibrary))
        return false;

    if(!loadGlobalLevelFunctions())
        return false;

    return true;
}

} //namespace VulkanSample
