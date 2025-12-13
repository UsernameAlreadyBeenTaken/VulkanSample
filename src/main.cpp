#include "VulkanApp.h"

int main()
{
  VulkanSample::VulkanApp app;

  VulkanSample::WindowParameters windowParameters = {};
  if(!VulkanSample::createWindowHandle(windowParameters, "VulkanSample", 50, 25, 1280, 800))
  {
      std::cerr << "Failed to create window handle" << std::endl;
      return -1;
  }

  if (!app.init(windowParameters))
  {
      std::cerr << "Error initializing Vulkan application, finishing execution..." << std::endl;
      return -1;
  }

  VulkanSample::destroyWindowHandle(windowParameters);

  return 0;
}
