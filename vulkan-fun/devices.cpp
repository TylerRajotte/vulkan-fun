#include "devices.hpp"

bool QueueFamilyIndices::isComplete(){
   return graphicsFamily.has_value() && presentFamily.has_value();
}

void devices::initDeviceSetup(VkSurfaceKHR* initSurface, const std::vector<const char*>* initDeviceExtensions, const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers){
    //Sets some pointers that are used by private functions and through the rest of the class
    pSurface = initSurface;
    pDeviceExtensions = initDeviceExtensions;
    pEnableValidationLayers = initEnableValidationLayers;
    pValidationLayers = initValidationLayers;
}

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
    
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(&device, pSurface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool devices::checkDeviceExtensionSupport(VkPhysicalDevice device){
    // Using a set of strings this checks if a device has support for the required extensions
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    // Maybe insert that vk_KHR_portability_subset thing in here with an if statement or something
    // https://stackoverflow.com/questions/66659907/vulkan-validation-warning-catch-22-about-vk-khr-portability-subset-on-moltenvk
    
    std::set<std::string> requiredExtensions(pDeviceExtensions->begin(), pDeviceExtensions->end());
    
    for (const auto& extension : availableExtensions){
        requiredExtensions.erase(extension.extensionName);
    }
    
    return requiredExtensions.empty();
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

void devices::createLogicalDevice(){
    // Filling out some structs for eventual logical device creation
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
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
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(pDeviceExtensions->size());
    createInfo.ppEnabledExtensionNames = pDeviceExtensions->data();
    
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
