#ifndef _VULKAN_APP_H_
#define _VULKAN_APP_H_

#include "Common.h"

namespace VulkanSample
{

class VulkanApp
{
public:
    bool init();

private:
    LIBRARY_TYPE m_VkLibrary;
    VkInstance   m_Instance;
};

} //namespace VulkanSample
#endif // _VULKAN_APP_H_