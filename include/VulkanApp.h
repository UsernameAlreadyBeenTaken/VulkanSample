#pragma once

#include "Common.h"

namespace VulkanSample
{

class VulkanApp
{
public:
    bool init();

private:
    LIBRARY_TYPE mVkLibrary;
    VkInstance   mInstance;
};

} //namespace VulkanSample
