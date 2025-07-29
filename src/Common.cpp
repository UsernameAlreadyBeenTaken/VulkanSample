#include "Common.h"

namespace VulkanSample
{

bool loadVkLibrary(LIBRARY_TYPE &vkLibrary)
{
#if defined _WIN32
    vkLibrary = LoadLibrary( "vulkan-1.dll" );
#elif defined __linux
    vkLibrary = dlopen( "libvulkan.so.1", RTLD_NOW );
#endif

    if (!vkLibrary)
    {
        std::cerr << "Failed to load vulkan library" << std::endl;
        return false;
    }
    return true;
}

bool loadFunctionFromVulkanLibrary(LIBRARY_TYPE const &vkLibrary)
{
#if defined _WIN32
  #define LoadFunction GetProcAddress
#elif defined __linux
  #define LoadFunction dlsym
#endif

#define EXPORTED_VULKAN_FUNCTION(name)                                \
    name = (PFN_##name)LoadFunction(vkLibrary, #name);                \
    if(name == nullptr) {                                             \
      std::cerr << "Could not load exported Vulkan function named: "  \
        #name << std::endl;                                           \
      return false;                                                   \
    }

#include "ListOfVulkanFunctions.inl"

    return true;
}

bool loadGlobalLevelFunctions()
{
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                \
    name = (PFN_##name)vkGetInstanceProcAddr(nullptr, #name);             \
    if(name == nullptr) {                                                 \
      std::cout << "Could not load global level Vulkan function named: "  \
        #name << std::endl;                                               \
      return false;                                                       \
    }

#include "ListOfVulkanFunctions.inl"

    return true;
}

bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &available_extensions)
{
  uint32_t extensions_count = 0;
  VkResult result = VK_SUCCESS;

  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
  if((result != VK_SUCCESS) || (extensions_count == 0))
  {
    std::cout << "Could not get the number of instance extensions." << std::endl;
    return false;
  }

  available_extensions.resize(extensions_count);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, available_extensions.data());
  if((result != VK_SUCCESS) || (extensions_count == 0))
  {
    std::cout << "Could not enumerate instance extensions." << std::endl;
    return false;
  }

  return true;
}

} // namespace VulkanSample