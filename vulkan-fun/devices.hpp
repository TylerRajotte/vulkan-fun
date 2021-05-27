#ifndef devices_hpp
#define devices_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

class devices{
public:
    void pickPhysicalDevice(VkInstance* pInstance);
    void createLogicalDevice(const bool* pEnableValidationLayers, const std::vector<const char*>* pValidationLayers);
    void destroyDevices();
private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};

#endif /* devices_hpp */
