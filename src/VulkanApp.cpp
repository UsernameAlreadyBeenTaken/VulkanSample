#include "VulkanApp.h"

namespace VulkanSample
{

bool VulkanApp::init()
{
    if (!loadVkLibrary(vkLibrary))
        return false;

    return true;
}

} //namespace VulkanSample
