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
      std::cerr << "Could not load global level Vulkan function named: "  \
        #name << std::endl;                                               \
      return false;                                                       \
    }

#include "ListOfVulkanFunctions.inl"

    return true;
}

bool checkAvailableInstanceExtensions(std::vector<VkExtensionProperties> &availableExtensions)
{
  uint32_t extensions_count = 0;
  VkResult result = VK_SUCCESS;

  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr);
  if((result != VK_SUCCESS) || (extensions_count == 0))
  {
    std::cerr << "Could not get the number of instance extensions." << std::endl;
    return false;
  }

  availableExtensions.resize(extensions_count);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, availableExtensions.data());
  if((result != VK_SUCCESS) || (extensions_count == 0))
  {
    std::cerr << "Could not enumerate instance extensions." << std::endl;
    return false;
  }

  return true;
}

bool isExtensionSupported(std::vector<VkExtensionProperties> const &availableExtensions, const char* const extension)
{
  for(auto &availableExtension : availableExtensions)
  {
    if(strstr(availableExtension.extensionName, extension))
    {
        return true;
    }
  }
  return false;
}

bool createInstance(std::vector<const char*> &desiredExtensions, const char* const appName, VkInstance &instance)
{
  std::vector<VkExtensionProperties> availableExtensions;
  if(!checkAvailableInstanceExtensions(availableExtensions))
    return false;

  for(auto &extension : desiredExtensions)
  {
    if(!isExtensionSupported(availableExtensions, extension))
    {
      std::cerr << "Extension named '" << extension << "' is not supported." << std::endl;
      return false;
    }
  }

  VkApplicationInfo applicationInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,   // VkStructureType           sType
    nullptr,                              // const void               *pNext
    appName,                              // const char               *pApplicationName
    VK_MAKE_VERSION(1, 0, 0),             // uint32_t                  applicationVersion
    "VulkanSample",                       // const char               *pEngineName
    VK_MAKE_VERSION(1, 0, 0),             // uint32_t                  engineVersion
    VK_API_VERSION_1_3                    // uint32_t                  apiVersion
  };

  VkInstanceCreateInfo instanceCreateInfo = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,             // VkStructureType           sType
    nullptr,                                            // const void               *pNext
    0,                                                  // VkInstanceCreateFlags     flags
    &applicationInfo,                                   // const VkApplicationInfo  *pApplicationInfo
    0,                                                  // uint32_t                  enabledLayerCount
    nullptr,                                            // const char * const       *ppEnabledLayerNames
    static_cast<uint32_t>(desiredExtensions.size()),    // uint32_t                  enabledExtensionCount
    desiredExtensions.data()                            // const char * const       *ppEnabledExtensionNames
  };

  VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
  if((result != VK_SUCCESS) || (instance == VK_NULL_HANDLE))
  {
    std::cerr << "Could not create Vulkan instance." << std::endl;
    return false;
  }

   return true;
}

bool loadInstanceLevelFunctions(VkInstance &instance, std::vector<char const *> const & enabledExtensions)
{
// Load core Vulkan API instance-level functions
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                  \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                \
    if(name == nullptr)                                                       \
    {                                                                         \
      std::cout << "Could not load instance-level Vulkan function named: "    \
        #name << std::endl;                                                   \
      return false;                                                           \
    }

// Load instance-level functions from enabled extensions
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)         \
    for(auto &enabledExtension : enabledExtensions)                          \
    {                                                                          \
      if(std::string(enabledExtension) == std::string(extension))             \
      {                                                                        \
        name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );           \
        if(name == nullptr)                                                    \
        {                                                                      \
          std::cout << "Could not load instance-level Vulkan function named: " \
            #name << std::endl;                                                \
          return false;                                                        \
        }                                                                      \
      }                                                                        \
    }

#include "ListOfVulkanFunctions.inl"

    return true;
}

bool enumerateAvailablePhysicalDevices(VkInstance &instance, std::vector<VkPhysicalDevice> &availableDevices)
{
  uint32_t devices_count = 0;
  VkResult result = VK_SUCCESS;

  result = vkEnumeratePhysicalDevices(instance, &devices_count, nullptr);
  if((result != VK_SUCCESS) || (devices_count == 0))
  {
    std::cout << "Could not get the number of available physical devices." << std::endl;
    return false;
  }

  availableDevices.resize(devices_count);
  result = vkEnumeratePhysicalDevices(instance, &devices_count, availableDevices.data());
  if((result != VK_SUCCESS) || (devices_count == 0))
  {
    std::cout << "Could not enumerate physical devices." << std::endl;
    return false;
  }

  return true;
}

bool checkAvailableDeviceExtensions(VkPhysicalDevice physicalDevice, std::vector<VkExtensionProperties> &availableExtensions)
{
  uint32_t extensionsCount = 0;
  VkResult result = VK_SUCCESS;

  result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr);
  if((result != VK_SUCCESS) || (extensionsCount == 0))
  {
    std::cout << "Could not get the number of device extensions." << std::endl;
    return false;
  }

  availableExtensions.resize(extensionsCount);
  result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data());
  if((result != VK_SUCCESS) || (extensionsCount == 0))
  {
    std::cout << "Could not enumerate device extensions." << std::endl;
    return false;
  }

  return true;
}

void getFeaturesAndPropertiesOfPhysicalDevice(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures &deviceFeatures,
                                              VkPhysicalDeviceProperties & deviceProperties)
{
  vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
}

} // namespace VulkanSample