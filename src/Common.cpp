#include <algorithm>

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
      std::cerr << "Could not load instance-level Vulkan function named: "    \
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
          std::cerr << "Could not load instance-level Vulkan function named: " \
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
    std::cerr << "Could not get the number of available physical devices." << std::endl;
    return false;
  }

  availableDevices.resize(devices_count);
  result = vkEnumeratePhysicalDevices(instance, &devices_count, availableDevices.data());
  if((result != VK_SUCCESS) || (devices_count == 0))
  {
    std::cerr << "Could not enumerate physical devices." << std::endl;
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
    std::cerr << "Could not get the number of device extensions." << std::endl;
    return false;
  }

  availableExtensions.resize(extensionsCount);
  result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data());
  if((result != VK_SUCCESS) || (extensionsCount == 0))
  {
    std::cerr << "Could not enumerate device extensions." << std::endl;
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
    std::cerr << "Could not get the number of queue families." << std::endl;
    return false;
  }

  queueFamilies.resize( queueFamiliesCount );
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilies.data());
  if(queueFamiliesCount == 0)
  {
    std::cerr << "Could not acquire properties of queue families." << std::endl;
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

bool selectQueueFamilyIndex(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, uint32_t &queueFamilyIndex)
{
  std::vector<VkQueueFamilyProperties> queueFamilies;
  uint32_t queueFamiliesCount = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
  if(queueFamiliesCount == 0)
  {
    std::cerr << "Could not get the number of queue families." << std::endl;
    return false;
  }

  queueFamilies.resize( queueFamiliesCount );
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, queueFamilies.data());
  if(queueFamiliesCount == 0)
  {
    std::cerr << "Could not acquire properties of queue families." << std::endl;
    return false;
  }

  for(uint32_t index = 0; index < static_cast<uint32_t>(queueFamilies.size()); ++index)
  {
    VkBool32 isPresentationSupported = VK_FALSE;
    VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, presentationSurface, &isPresentationSupported);
    if((result == VK_SUCCESS) && (isPresentationSupported == VK_TRUE))
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
    std::cerr << "Could not load device-level Vulkan function named: " #name << std::endl; \
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
          std::cerr << "Could not load device-level Vulkan function named: " #name << std::endl; \
          return false;                                                                          \
        }                                                                                        \
      }                                                                                          \
    }

#include "ListOfVulkanFunctions.inl"

  return true;
}

bool createLogicalDevice(VkInstance instance, VkDevice &logicalDevice, std::vector<const char*> &desiredExtensions, VkSurfaceKHR surface,
                         QueueParameters &graphicsQueue, QueueParameters &computeQueue, QueueParameters &presentQueue)
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

    uint32_t presentQueueFamilyIndex;
    if(!selectQueueFamilyIndex(physicalDevice, surface, presentQueueFamilyIndex))
    {
      continue;
    }

    std::vector<QueueInfo> requestedQueues = { {graphicsQueueFamilyIndex, {1.0f}} };
    auto insertIfUnique = [](std::vector<QueueInfo> &vec, QueueInfo item)
    {
      if (std::find(vec.begin(), vec.end(), item) == vec.end())
      {
        vec.push_back(item);
      }
    };

    insertIfUnique(requestedQueues, {computeQueueFamilyIndex, { 1.0f }});
    insertIfUnique(requestedQueues, {presentQueueFamilyIndex, { 1.0f }});

    std::vector<VkExtensionProperties> availableExtensions;
    if(!checkAvailableDeviceExtensions(physicalDevice, availableExtensions))
    {
      continue;
    }

    for(auto &extension : desiredExtensions)
    {
      if(!isExtensionSupported(availableExtensions, extension))
      {
        std::cerr << "Extension named '" << extension << "' is not supported by a physical device." << std::endl;
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
      std::cerr << "Could not create logical device." << std::endl;
      continue;
    }

    if(!loadDeviceLevelFunctions(logicalDevice, {}))
    {
      return false;
    }
    vkGetDeviceQueue(logicalDevice, graphicsQueueFamilyIndex, 0, &graphicsQueue.handle);
    vkGetDeviceQueue(logicalDevice, computeQueueFamilyIndex, 0, &computeQueue.handle);
    vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &presentQueue.handle);
    return true;
  }

  return false;
}

bool createPresentationSurface(VkInstance instance, WindowParameters windowParameters, VkSurfaceKHR presentationSurface)
{
  VkResult result = VK_RESULT_MAX_ENUM;

#ifdef VK_USE_PLATFORM_WIN32_KHR
  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
  {
    VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,  // VkStructureType                 sType
    nullptr,                                          // const void                    * pNext
    0,                                                // VkWin32SurfaceCreateFlagsKHR    flags
    windowParameters.HInstance,                      // HINSTANCE                       hinstance
    windowParameters.HWnd                            // HWND                            hwnd
  };
  result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);
#elif defined VK_USE_PLATFORM_XLIB_KHR
  VkXlibSurfaceCreateInfoKHR surfaceCreateInfo =
  {
    VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,   // VkStructureType                 sType
    nullptr,                                          // const void                    * pNext
    0,                                                // VkXlibSurfaceCreateFlagsKHR     flags
    windowParameters.Dpy,                            // Display                       * dpy
    windowParameters.Window                          // Window                          window
  };
  result = vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);
#elif defined VK_USE_PLATFORM_XCB_KHR
  VkXcbSurfaceCreateInfoKHR surfaceCreateInfo =
  {
    VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,    // VkStructureType                 sType
    nullptr,                                          // const void                    * pNext
    0,                                                // VkXcbSurfaceCreateFlagsKHR      flags
    window_parameters.Connection,                     // xcb_connection_t              * connection
    window_parameters.Window                          // xcb_window_t                    window
  };
  result = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &presentationSurface);
#endif

  if((VK_SUCCESS != result) || (VK_NULL_HANDLE == presentationSurface))
  {
    std::cerr << "Could not create presentation surface." << std::endl;
    return false;
  }
  return true;
}

bool selectPresentationMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkPresentModeKHR desiredMode, 
                            VkPresentModeKHR &presentMode)
{
  uint32_t presentModesCount = 0;
  VkResult result = VK_SUCCESS;

  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, nullptr);
  if((result != VK_SUCCESS) || (presentModesCount == 0))
  {
    std::cerr << "Could not get the number of supported present modes." << std::endl;
    return false;
  }

  std::vector<VkPresentModeKHR> presentModes(presentModesCount);
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, presentationSurface, &presentModesCount, presentModes.data());
  if((result != VK_SUCCESS) || (presentModesCount == 0))
  {
    std::cerr << "Could not enumerate present modes." << std::endl;
    return false;
  }

  for(auto &currentPresentMode : presentModes)
  {
    if(currentPresentMode == desiredMode)
    {
      presentMode = desiredMode;
      return true;
    }
  }

  std::cout << "Desired present mode is not supported. Selecting default FIFO mode." << std::endl;
  for(auto &currentPresentMode : presentModes)
  {
    if(currentPresentMode == VK_PRESENT_MODE_FIFO_KHR)
    {
      presentMode = VK_PRESENT_MODE_FIFO_KHR;
      return true;
    }
  }

  std::cerr << "VK_PRESENT_MODE_FIFO_KHR is not supported though it's mandatory for all drivers!" << std::endl;
  return false;
}

bool selectSwapchainImageFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface,
                                VkSurfaceFormatKHR desiredSurfaceFormat, VkFormat &imageFormat, VkColorSpaceKHR &imageColorSpace)
{
  uint32_t formatsCount = 0;
  VkResult result = VK_SUCCESS;

  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatsCount, nullptr);
  if((result != VK_SUCCESS) || (formatsCount == 0))
  {
    std::cerr << "Could not get the number of supported surface formats." << std::endl;
    return false;
  }

  std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, presentationSurface, &formatsCount, surfaceFormats.data());
  if((result != VK_SUCCESS) || (formatsCount == 0))
  {
    std::cerr << "Could not enumerate supported surface formats." << std::endl;
    return false;
  }

  if((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
  {
    imageFormat = desiredSurfaceFormat.format;
    imageColorSpace = desiredSurfaceFormat.colorSpace;
    return true;
  }

  for(auto &surfaceFormat : surfaceFormats)
  {
    if((desiredSurfaceFormat.format == surfaceFormat.format) && (desiredSurfaceFormat.colorSpace == surfaceFormat.colorSpace))
    {
      imageFormat = desiredSurfaceFormat.format;
      imageColorSpace = desiredSurfaceFormat.colorSpace;
      return true;
    }
  }

  for(auto &surfaceFormat : surfaceFormats)
  {
    if((desiredSurfaceFormat.format == surfaceFormat.format))
    {
      imageFormat = desiredSurfaceFormat.format;
      imageColorSpace = surfaceFormat.colorSpace;
      std::cout << "Desired combination of format and colorspace is not supported. Selecting other colorspace." << std::endl;
      return true;
    }
  }

  imageFormat = surfaceFormats[0].format;
  imageColorSpace = surfaceFormats[0].colorSpace;
  std::cout << "Desired format is not supported. Selecting available format - colorspace combination." << std::endl;
  return true;
}

bool createSwapchain(VkPhysicalDevice physicalDevice, VkSurfaceKHR presentationSurface, VkDevice logicalDevice,
                      VkPresentModeKHR desiredMode, VkSurfaceFormatKHR desiredSurfaceFormat,
                      VkSurfaceTransformFlagBitsKHR desiredTransform, VkImageUsageFlags swapchainImageUsage,
                      VkExtent2D &imageSize, VkFormat &imageFormat, VkSwapchainKHR &oldSwapchain, VkSwapchainKHR &swapchain, 
                      std::vector<VkImage> &swapchainImages)
{
  VkPresentModeKHR presentMode;
  if (!selectPresentationMode(physicalDevice, presentationSurface, desiredMode, presentMode))
    return false;

  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, presentationSurface, &surfaceCapabilities);
  if(result != VK_SUCCESS)
  {
    std::cerr << "Could not get the capabilities of a presentation surface." << std::endl;
    return false;
  }

  uint32_t imagesNumber = surfaceCapabilities.minImageCount + 1;
  if((surfaceCapabilities.maxImageCount > 0) && (imagesNumber > surfaceCapabilities.maxImageCount))
    imagesNumber = surfaceCapabilities.maxImageCount;

  if(surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
  {
    imageSize = {640, 480};

    if(imageSize.width < surfaceCapabilities.minImageExtent.width)
      imageSize.width = surfaceCapabilities.minImageExtent.width;
    else if(imageSize.width > surfaceCapabilities.maxImageExtent.width)
      imageSize.width = surfaceCapabilities.maxImageExtent.width;

    if(imageSize.height < surfaceCapabilities.minImageExtent.height)
      imageSize.height = surfaceCapabilities.minImageExtent.height;
    else if(imageSize.height > surfaceCapabilities.maxImageExtent.height)
      imageSize.height = surfaceCapabilities.maxImageExtent.height;
  }
  else
  {
    imageSize = surfaceCapabilities.currentExtent;
  }

  if((imageSize.width == 0) || (imageSize.height == 0))
    return true;

  VkImageUsageFlags imageUsage = swapchainImageUsage & surfaceCapabilities.supportedUsageFlags;
  if(swapchainImageUsage != imageUsage)
  {
    std::cerr << "Surface doesn`t support such image usage flags." << std::endl;
    return false;
  }

  VkSurfaceTransformFlagBitsKHR surfaceTransform;
  if(surfaceCapabilities.supportedTransforms & desiredTransform)
    surfaceTransform = desiredTransform;
  else
    surfaceTransform = surfaceCapabilities.currentTransform;

  VkColorSpaceKHR imageColorSpace;
  if(!selectSwapchainImageFormat(physicalDevice, presentationSurface, desiredSurfaceFormat, imageFormat, imageColorSpace))
    return false;

  VkSurfaceFormatKHR surfaceFormat = {imageFormat, imageColorSpace};
  VkSwapchainCreateInfoKHR swapchainCreateInfo = {
    VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,  // VkStructureType                  sType
    nullptr,                                      // const void                     * pNext
    0,                                            // VkSwapchainCreateFlagsKHR        flags
    presentationSurface,                          // VkSurfaceKHR                     surface
    imagesNumber,                                 // uint32_t                         minImageCount
    surfaceFormat.format,                         // VkFormat                         imageFormat
    surfaceFormat.colorSpace,                     // VkColorSpaceKHR                  imageColorSpace
    imageSize,                                    // VkExtent2D                       imageExtent
    1,                                            // uint32_t                         imageArrayLayers
    imageUsage,                                   // VkImageUsageFlags                imageUsage
    VK_SHARING_MODE_EXCLUSIVE,                    // VkSharingMode                    imageSharingMode
    0,                                            // uint32_t                         queueFamilyIndexCount
    nullptr,                                      // const uint32_t                 * pQueueFamilyIndices
    surfaceTransform,                             // VkSurfaceTransformFlagBitsKHR    preTransform
    VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,            // VkCompositeAlphaFlagBitsKHR      compositeAlpha
    presentMode,                                  // VkPresentModeKHR                 presentMode
    VK_TRUE,                                      // VkBool32                         clipped
    oldSwapchain                                  // VkSwapchainKHR                   oldSwapchain
  };

  result = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &swapchain);
  if((result != VK_SUCCESS) || (swapchain == VK_NULL_HANDLE))
  {
    std::cerr << "Could not create a swapchain." << std::endl;
    return false;
  }

  if(oldSwapchain != VK_NULL_HANDLE)
  {
    vkDestroySwapchainKHR(logicalDevice, oldSwapchain, nullptr);
    oldSwapchain = VK_NULL_HANDLE;
  }

  uint32_t imagesCount = 0;
  result = VK_SUCCESS;

  result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imagesCount, nullptr);
  if( (result != VK_SUCCESS) || (imagesCount == 0))
  {
    std::cerr << "Could not get the number of swapchain images." << std::endl;
    return false;
  }

  swapchainImages.resize(imagesCount);
  result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imagesCount, swapchainImages.data());
  if((result != VK_SUCCESS) || (imagesCount == 0))
  {
    std::cerr << "Could not enumerate swapchain images." << std::endl;
    return false;
  }

  return true;
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