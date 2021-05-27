#include "devices.hpp"

void devices::pickPhysicalDevice(VkInstance* pInstance, VkSurfaceKHR* initSurface) {
    // Goes through all the avaliable devices and makes sure we use a valid and proper one that will work
    
    // Also I guess im going to treat this like an init function for the devices class so I can get the to use it later
    pSurface = initSurface;
    
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
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *pSurface, &presentSupport);
        
        if(presentSupport){
            indices.presentFamily = i;
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
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    // Might be used layer to get other features like a swap chain
    VkPhysicalDeviceFeatures deviceFeatures{};
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    
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
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void devices::destroyDevices(){
    vkDestroyDevice(device, nullptr);
}
