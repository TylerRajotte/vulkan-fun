#include "querySwapchainSupport.hpp"

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice* pPhysicalDevice, VkSurfaceKHR* pSurface){
    //Figures out what formats and present modes that the device/surface supports and returns a struct that contains that info
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pPhysicalDevice, *pSurface, &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice, *pSurface, &formatCount, nullptr);
    
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(*pPhysicalDevice, *pSurface, &formatCount, details.formats.data());
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(*pPhysicalDevice, *pSurface, &presentModeCount, nullptr);
    
    if (presentModeCount != 0){
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(*pPhysicalDevice, *pSurface, &presentModeCount, details.presentModes.data());
    }
    
    return details;
}
