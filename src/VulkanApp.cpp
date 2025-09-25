#include "VulkanApp.h"

namespace VulkanSample
{

bool VulkanApp::init()
{
    if (!loadVkLibrary(m_VkLibrary))
        return false;

    if (!loadFunctionFromVulkanLibrary(m_VkLibrary))
        return false;

    if(!loadGlobalLevelFunctions())
        return false;

    if (!createInstance({}, "VulkanSample", m_Instance))
        return false;

    if (!loadInstanceLevelFunctions(m_Instance, {}))
        return false;

    // enumerate phys devices + check their extensions

    return true;
}

} //namespace VulkanSample
