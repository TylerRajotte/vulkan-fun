#ifndef vulkan_hpp
#define vulkan_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include "debugMessengerUtil.hpp"
#include "windowManager.hpp"
#include "devices.hpp"
#include "swapchain.hpp"
#include "graphicsPipeline.hpp"
#include "renderPass.hpp"
#include "frameBuffer.hpp"
#include "commands.hpp"

class vulkan{
public:
    void initVulkan(const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers, const std::vector<const char*>* initDeviceExensions, windowManager* initWindow, const int* initMaxFramesInFlight);
    void drawFrame();
    void destroyVulkan();
    
    devices devices;
private:
    VkInstance instance;
    VkSurfaceKHR surface;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame;
    
    debugMessengerUtil debugMessengerUtil;
    swapchain swapchain;
    renderPass renderPass;
    graphicsPipeline graphicsPipeline;
    framebuffer framebuffer;
    commands commands;
    
    const bool* pEnableValidationLayers;
    const int* pMaxFramesInFlight;
    const std::vector<const char*>* pValidationLayers;
    const std::vector<const char*>* pDeviceExtensions;
    windowManager* pWindow;
    
    void createSyncObjects();
    bool checkValidationLayerSupport();
    void createInstance();
    void createSurface();
};

#endif /* vulkan_hpp */
