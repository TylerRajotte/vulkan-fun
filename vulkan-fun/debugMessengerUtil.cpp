#include "debugMessengerUtil.hpp"

// Have to setup these functions manually because it isn't included by default
VkResult debugMessengerUtil::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr){
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void debugMessengerUtil::DestoryDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr){
        func(instance, debugMessenger, pAllocator);
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerUtil::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData){
    // Whenever the validation layer decided to talk back via the callback just throw it into a cerr 
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
    
    return VK_FALSE;
}

void debugMessengerUtil::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
    // Broken out to its own function so that it can be reused in multiple places
    // Fill out the settings structure with what we want, maybe this could be moved to a settings.txt file at some point
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    // Maybe not needed createInfo.pUserData = nullptr;
}

void debugMessengerUtil::setupDebugMessenger(const bool* pEnableValidationLayers, VkInstance* pInstance){
    if (!*pEnableValidationLayers) return;
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    
    // Creates the actual messenger mechanism
    if(CreateDebugUtilsMessengerEXT(*pInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS){
        throw std::runtime_error("failed to setup debug messenger");
    }
}

void debugMessengerUtil::destroyDebugMessengerUtil(VkInstance* pInstance){
    DestoryDebugUtilsMessengerEXT(*pInstance, debugMessenger, nullptr);
}
