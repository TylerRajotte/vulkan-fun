#include "devices.hpp"

void devices::pickPhysicalDevice(VkInstance* pInstance) {
    // Goes through all the avaliable devices and makes sure we use a valid and proper one that will work
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

void devices::createLogicalDevice(const bool* pEnableValidationLayers, const std::vector<const char*>* pValidationLayers){
    // Filling out some structs for eventual logical device creation
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    
    // Might be used layer to get other features like a swap chain
    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    createInfo.enabledExtensionCount = 0;
    
    // Validation Layers for the logical device if they are enabled to begin with
    if(*pEnableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(pValidationLayers->size());
        createInfo.ppEnabledLayerNames = pValidationLayers->data();
    } else {
        createInfo.enabledLayerCount = 0;
    }
    
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
        throw std::runtime_error("Failed to create logical device!");
    }
    
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

void devices::destroyDevices(){
    vkDestroyDevice(device, nullptr);
}
