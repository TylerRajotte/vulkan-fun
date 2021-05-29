#ifndef vulkan_hpp
#define vulkan_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <cstring>
#include <iostream>
#include "debugMessengerUtil.hpp"
#include "windowManager.hpp"
#include "devices.hpp"
#include "swapchain.hpp"

class vulkan{
public:
    void initVulkan(const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers, const std::vector<const char*>* initDeviceExensions, windowManager* initWindow);
    void destroyVulkan();
private:
    VkInstance instance;
    VkSurfaceKHR surface;
    
    debugMessengerUtil debugMessengerUtil;
    devices devices;
    swapchain swapchain;
    
    const bool* pEnableValidationLayers;
    const std::vector<const char*>* pValidationLayers;
    const std::vector<const char*>* pDeviceExtensions;
    windowManager* pWindow;
    
    bool checkValidationLayerSupport();
    void createInstance();
    void createSurface();
};

#endif /* vulkan_hpp */
