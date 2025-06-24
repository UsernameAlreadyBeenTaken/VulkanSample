#include "Common.h"
#include "VulkanFunctions.h"

namespace VulkanSample
{

class VulkanApp
{
public:
    bool init();

private:
    LIBRARY_TYPE vkLibrary;
};

} //namespace VulkanSample