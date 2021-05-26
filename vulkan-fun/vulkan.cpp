#include "vulkan.hpp"

void vulkan::initVulkan(const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers, windowManager* initWindow){
    pEnableValidationLayers = initEnableValidationLayers;
    pValidationLayers = initValidationLayers;
    pWindow = initWindow;
    
    createInstance();
    debugMessengerUtil.setupDebugMessenger(pEnableValidationLayers, &instance);
    pickPhysicalDevice();
}

void vulkan::pickPhysicalDevice() {
    
}

bool vulkan::checkValidationLayerSupport() {
    // Gathering some basic data of whats available
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    // Checking if all the layers in validation layers exist in the available layers list
    for (const char* layerName : *pValidationLayers){
        bool layerFound = false;
        
        for (const auto& layerProperties : availableLayers){
            if (strcmp(layerName, layerProperties.layerName) == 0){
                layerFound = true;
                break;
            }
        }
        
        if (!layerFound) {
            return false;
        }
    }
    
    return true;
}

void vulkan::createInstance(){
    if (*pEnableValidationLayers && !checkValidationLayerSupport()){
        throw std::runtime_error("validation layers requested, but not available");
    }
    
    // Fill out the basic info into a special stucture object, maybe something to be moved to a settings.txt style file
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Pass the previous structure it into the createinfo structure
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // Call the function that figures out what glfw needs to do its thing
    auto extensions = pWindow->getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    // Some extra code to let vulkan know if validation layers have been enabled or not and to pass the relevent info through
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (*pEnableValidationLayers){        
        createInfo.enabledLayerCount = static_cast<uint32_t>(pValidationLayers->size());
        createInfo.ppEnabledLayerNames = pValidationLayers->data();
        
        // Some extra stuff so that we can get debug messengers when creating the instance
        debugMessengerUtil.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        
        createInfo.pNext = nullptr;
    }

    // Actually make the instance now that it has its filled out checklist of every detail to ever exist
    // Also Error checking for if any of the stuff above is causing trouble
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        throw std::runtime_error("Failed to create instance");
    }
}


void vulkan::destroyVulkan(){
    // Clean up the messenger system if validation layers are enabled
    if (*pEnableValidationLayers) {
        debugMessengerUtil.destroyDebugMessengerUtil(&instance);
    }
    
    // General Cleanup to free memory
    vkDestroyInstance(instance, nullptr);
}
