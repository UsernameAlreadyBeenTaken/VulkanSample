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
  uint32_t extensionsCount = 0;
  VkResult result = VK_SUCCESS;

  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
  if((result != VK_SUCCESS) || (extensionsCount == 0))
  {
    std::cerr << "Could not get the number of instance extensions." << std::endl;
    return false;
  }

  availableExtensions.resize(extensionsCount);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, availableExtensions.data());
  if((result != VK_SUCCESS) || (extensionsCount == 0))
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

bool isLayerSupported(const char* desiredLayer)
{
  uint32_t layerCount = 0;
  VkResult result = VK_SUCCESS;
  result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  if((result != VK_SUCCESS) || (layerCount == 0))
  {
    std::cerr << "Could not get the number of instance layers." << std::endl;
    return false;
  }

  std::vector<VkLayerProperties> availableLayers(layerCount);
  result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  if((result != VK_SUCCESS) || (layerCount == 0))
  {
    std::cerr << "Could not enumerate instance layers." << std::endl;
    return false;
  }

  for (auto &layer : availableLayers)
  {
    if (strstr(layer.layerName, desiredLayer))
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

  std::vector<const char*> desiredLayers = {};
#ifdef _DEBUG
  desiredLayers.emplace_back("VK_LAYER_KHRONOS_validation");
  for(auto layer : desiredLayers)
  {
    if(!isLayerSupported(layer))
    {
      std::cerr << "Layer " << layer << " requested but not found" << std::endl;
      return false;
    }
  }
#endif

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
    static_cast<uint32_t>(desiredLayers.size()),        // uint32_t                  enabledLayerCount
    desiredLayers.data(),                               // const char * const       *ppEnabledLayerNames
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

bool selectQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkQueueFlags desiredCapabilities, uint32_t &queueFamilyIndex)
{
  std::vector<VkQueueFamilyProperties> queueFamilies;
  uint32_t queueFamiliesCount = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
  if(queueFamiliesCount == 0)
  {
    std::cout << "Could not get the number of queue families." << std::endl;
    return false;
  }

  queueFamilies.resize( queueFamiliesCount );
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilies.data());
  if(queueFamiliesCount == 0)
  {
    std::cout << "Could not acquire properties of queue families." << std::endl;
    return false;
  }

  for (uint32_t index = 0; index < static_cast<uint32_t>(queueFamilies.size()); ++index)
  {
    if (queueFamilies[index].queueCount > 0 && ((queueFamilies[index].queueFlags & desiredCapabilities) == desiredCapabilities))
    {
      queueFamilyIndex = index;
      return true;
    }
  }

  return false;
}

bool loadDeviceLevelFunctions(VkDevice logicalDevice, std::vector<const char *> const &enabledExtensions)
{
  // Load core Vulkan API device-level functions
#define DEVICE_LEVEL_VULKAN_FUNCTION(name)                                                 \
  name = (PFN_##name)vkGetDeviceProcAddr(logicalDevice, #name);                            \
  if(name == nullptr)                                                                      \
  {                                                                                        \
    std::cout << "Could not load device-level Vulkan function named: " #name << std::endl; \
    return false;                                                                          \
  }

  // Load device-level functions from enabled extensions
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)          \
    for(auto &enabledExtension : enabledExtensions) \
    {                      \
      if(std::string(enabledExtension) == std::string(extension))                                \
      {                                                                                          \
        name = (PFN_##name)vkGetDeviceProcAddr(logicalDevice, #name);                            \
        if(name == nullptr)                                                                      \
        {                                                                                        \
          std::cout << "Could not load device-level Vulkan function named: " #name << std::endl; \
          return false;                                                                          \
        }                                                                                        \
      }                                                                                          \
    }

#include "ListOfVulkanFunctions.inl"

  return true;
}

bool createLogicalDevice(VkInstance instance, VkDevice &logicalDevice, VkQueue &graphicsQueue, VkQueue &computeQueue)
{

  std::vector<VkPhysicalDevice> physicalDevices;
  enumerateAvailablePhysicalDevices(instance, physicalDevices);

  for(auto &physicalDevice : physicalDevices)
  {
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceProperties deviceProperties;

    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    if(!deviceFeatures.geometryShader)
    {
      continue;
    }
    else
    {
      deviceFeatures = {};
      deviceFeatures.geometryShader = VK_TRUE;
    }

    uint32_t graphicsQueueFamilyIndex;
    if(!selectQueueFamilyIndex(physicalDevice, VK_QUEUE_GRAPHICS_BIT, graphicsQueueFamilyIndex))
    {
      continue;
    }

    uint32_t computeQueueFamilyIndex;
    if(!selectQueueFamilyIndex(physicalDevice, VK_QUEUE_COMPUTE_BIT, computeQueueFamilyIndex))
    {
      continue;
    }

    std::vector<QueueInfo> requestedQueues = { {graphicsQueueFamilyIndex, {1.0f}} };
    if(graphicsQueueFamilyIndex != computeQueueFamilyIndex)
    {
      requestedQueues.push_back( {computeQueueFamilyIndex, { 1.0f }} );
    }

    std::vector<const char*> const &desiredExtensions = {};

    std::vector<VkExtensionProperties> availableExtensions;
    if(!checkAvailableDeviceExtensions(physicalDevice, availableExtensions))
    {
      continue;
    }

    for(auto &extension : desiredExtensions)
    {
      if(!isExtensionSupported(availableExtensions, extension))
      {
        std::cout << "Extension named '" << extension << "' is not supported by a physical device." << std::endl;
        continue;
      }
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for(auto & info : requestedQueues)
    {
      queueCreateInfos.push_back({
          VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // VkStructureType             sType
          nullptr,                                        // const void                * pNext
          0,                                              // VkDeviceQueueCreateFlags    flags
          info.familyIndex,                               // uint32_t                    queueFamilyIndex
          static_cast<uint32_t>(info.priorities.size()),  // uint32_t                    queueCount
          info.priorities.data()                          // const float               * pQueuePriorities
        });
    };

    VkDeviceCreateInfo deviceCreateInfo = {
      VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,             // VkStructureType                  sType
      nullptr,                                          // const void                     * pNext
      0,                                                // VkDeviceCreateFlags              flags
      static_cast<uint32_t>(queueCreateInfos.size()),   // uint32_t                         queueCreateInfoCount
      queueCreateInfos.data(),                          // const VkDeviceQueueCreateInfo  * pQueueCreateInfos
      0,                                                // uint32_t                         enabledLayerCount
      nullptr,                                          // const char * const             * ppEnabledLayerNames
      static_cast<uint32_t>(desiredExtensions.size()), // uint32_t                         enabledExtensionCount
      desiredExtensions.data(),                        // const char * const             * ppEnabledExtensionNames
      &deviceFeatures                                  // const VkPhysicalDeviceFeatures * pEnabledFeatures
    };

    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice );
    if((result != VK_SUCCESS) || (logicalDevice == VK_NULL_HANDLE))
    {
      std::cout << "Could not create logical device." << std::endl;
      continue;
    }

    if(!loadDeviceLevelFunctions(logicalDevice, {}))
    {
      return false;
    }
    vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, &computeQueue);
    return true;
  }

  return false;
}

void destroyVulkanObjects(VkDevice logicalDevice, VkInstance instance)
{
  if(logicalDevice)
  {
    vkDestroyDevice(logicalDevice, nullptr);
    logicalDevice = VK_NULL_HANDLE;
  }

  if(instance)
  {
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
  }
}

void releaseVulkanLibrary(LIBRARY_TYPE &vulkanLibrary)
{
  if(vulkanLibrary != nullptr)
  {
#if defined _WIN32
    FreeLibrary(vulkanLibrary);
#elif defined __linux
    dlclose(vulkanLibrary);
#endif
    vulkanLibrary = nullptr;
  }
}

} // namespace VulkanSample