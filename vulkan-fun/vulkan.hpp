#ifndef vulkan_hpp
#define vulkan_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <cstring>
#include "debugMessengerUtil.hpp"
#include "windowManager.hpp"

class vulkan{
public:
    void initVulkan(const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers, windowManager* initWindow);
    void destroyVulkan();
private:
    VkInstance instance;
    debugMessengerUtil debugMessengerUtil;
    
    const bool* pEnableValidationLayers;
    const std::vector<const char*>* pValidationLayers;
    windowManager* pWindow;
    
    void pickPhysicalDevice();
    bool checkValidationLayerSupport();
    void createInstance();
};

#endif /* vulkan_hpp */
