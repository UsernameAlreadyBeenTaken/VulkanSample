#pragma once

#include "Common.h"

namespace VulkanSample
{

class VulkanApp
{
public:
    VulkanApp();
    ~VulkanApp();
    bool init(WindowParameters windowParameters);

private:
    LIBRARY_TYPE     mVkLibrary;
    VkInstance       mInstance;
    VkSurfaceKHR     mSurface;
    VkDevice         mLogicalDevice;
    QueueParameters  mGraphicsQueue;
    QueueParameters  mComputeQueue;
    QueueParameters  mPresentQueue;
};

} //namespace VulkanSample
