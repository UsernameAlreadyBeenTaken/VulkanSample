#include "VulkanApp.h"

int main()
{
  VulkanSample::WindowParameters windowParameters = {};
  if(!VulkanSample::createWindowHandle(windowParameters, "VulkanSample", 50, 25, 1280, 800))
  {
      std::cerr << "Failed to create window handle" << std::endl;
      return -1;
  }

  VulkanSample::VulkanApp app;
  if (!app.init(windowParameters))
  {
      std::cerr << "Error initializing Vulkan application, finishing execution..." << std::endl;
      return -1;
  }

  VulkanSample::destroyWindowHandle(windowParameters);

  return 0;
}
