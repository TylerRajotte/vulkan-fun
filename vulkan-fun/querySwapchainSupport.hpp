#ifndef querySwapchainSupport_hpp
#define querySwapchainSupport_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice* pPhysicalDevice, VkSurfaceKHR* pSurface);

#endif /* querySwapchainSupport_hpp */
