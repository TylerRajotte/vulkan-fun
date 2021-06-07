#ifndef devices_hpp
#define devices_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <optional>
#include <set>
#include <string>
#include "querySwapchainSupport.hpp"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete();
};

class devices{
public:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    
    void initDeviceSetup(VkSurfaceKHR* initSurface, const std::vector<const char*>* initDeviceExtensions, const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers);
    void pickPhysicalDevice(VkInstance* pInstance);
    void createLogicalDevice();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void destroyDevices();
private:
    VkSurfaceKHR* pSurface;
    const std::vector<const char*>* pDeviceExtensions;
    const bool* pEnableValidationLayers;
    const std::vector<const char*>* pValidationLayers;
    
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool isPortableSpec(VkPhysicalDevice);
};

#endif /* devices_hpp */
