#include "devices.hpp"

void devices::pickPhysicalDevice(VkInstance* pInstance) {
    // Goes through all the avaliable devices and makes sure we use a valid and proper one that will work
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*pInstance, &deviceCount, nullptr);
    
    if (deviceCount == 0){
        throw std::runtime_error("Failed to find gpus with vulkan support!");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*pInstance, &deviceCount, devices.data());
    
    for (const auto& device : devices){
        if(isDeviceSuitable(device)){
            physicalDevice = device;
            break;
        }
    }
    
    if (physicalDevice == VK_NULL_HANDLE){
        throw std::runtime_error("Failed to find suitable GPU!");
    }
}

bool devices::isDeviceSuitable(VkPhysicalDevice device){
    // The check for the indiviual devices, could possibly add more here for better filtering
    QueueFamilyIndices indices = findQueueFamilies(device);
    
    return indices.isComplete();
}

QueueFamilyIndices devices::findQueueFamilies(VkPhysicalDevice device){
    // Sorts through the queues that the device supports and makes sure it supports VK_QUEUE_GRAPHICS_BIT
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    int i = 0;
    for (const auto& queueFamily : queueFamilies){
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamily = i;
        }
        
        if (indices.isComplete()){
            break;
        }
        
        i++;
    }
    
    return indices;
}
