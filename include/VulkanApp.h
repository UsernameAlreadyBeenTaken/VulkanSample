#pragma once

#include "Common.h"

namespace VulkanSample
{

class VulkanApp
{
public:
    ~VulkanApp();
    bool init(WindowParameters windowParameters);

private:
    LIBRARY_TYPE  mVkLibrary;
    VkInstance    mInstance;
    VkDevice      mLogicalDevice;
    VkSurfaceKHR  mSurface;
};

} //namespace VulkanSample
