#ifndef swapchain_hpp
#define swapchain_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <algorithm>
#include "windowManager.hpp"
#include "devices.hpp"
#include "querySwapchainSupport.hpp"

class swapchain {
public:
    void createSwapChain(windowManager* initWindow, devices* initDevices, VkSurfaceKHR* initSurface);
    void createImageViews();
    void destroySwapChain();
    
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;
private:
    windowManager* pWindow;
    devices* pDevices;
    VkSurfaceKHR* pSurface;
    
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};

#endif /* swapchain_hpp */
