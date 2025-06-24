#include "VulkanApp.h"

int main()
{
    VulkanSample::VulkanApp app;
    if (!app.init())
    {
        std::cerr << "Error initializing Vulkan application, finishing execution..." << std::endl;
        return -1;
    }

    return 0;
}