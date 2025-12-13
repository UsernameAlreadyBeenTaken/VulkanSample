#include "VulkanApp.h"

int main()
{
    VulkanSample::VulkanApp app;
    VulkanSample::WindowParameters windowParameters;

    if (!app.init(windowParameters))
    {
        std::cerr << "Error initializing Vulkan application, finishing execution..." << std::endl;
        return -1;
    }

    return 0;
}