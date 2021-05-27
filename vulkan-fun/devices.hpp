#ifndef devices_hpp
#define devices_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <optional>
#include <set>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class devices{
public:
    void pickPhysicalDevice(VkInstance* pInstance, VkSurfaceKHR* initSurface);
    void createLogicalDevice(const bool* pEnableValidationLayers, const std::vector<const char*>* pValidationLayers);
    void destroyDevices();
private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR* pSurface;
    
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

#endif /* devices_hpp */
