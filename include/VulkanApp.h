#pragma once

#include "Common.h"

namespace VulkanSample
{

class VulkanApp
{
public:
    ~VulkanApp();
    bool init();

private:
    LIBRARY_TYPE mVkLibrary;
    VkInstance   mInstance;
    VkDevice     mLogicalDevice;
};

} //namespace VulkanSample
