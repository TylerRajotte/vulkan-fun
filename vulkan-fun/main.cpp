#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

// Width and Height of the window
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

// Fancy code to toggle validation layers when compiling a debug build
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers= true;
#endif

// Have to setup these functions manually because it isn't included by default
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger){
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr){
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestoryDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator){
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr){
        func(instance, debugMessenger, pAllocator);
    }
}

class HelloTriangleApplication{
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
private:
    // Some Global Variables that are used throughout the program
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    void initWindow(){
        // Basic GLFW Setup code
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    }

    void initVulkan(){
        createInstance();
        setupDebugMessenger();
    }
    
    void setupDebugMessenger(){
        if (!enableValidationLayers) return;
        
        // Fill out the settings structure with what we want, maybe this could be moved to a settings.txt file at some point
        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
        
        //Creates the actual messenger mechanism
        if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS){
            throw std::runtime_error("failed to setup debug messenger");
        }
    }
    
    std::vector<const char*> getRequiredExtensions() {
        // Handy Function to talk with glfw and figure out what it needs from vulkan
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        
        return extensions;
    }
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData){
        // Allows the validations layers to actually talk a bit, I think
        std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
        
        return VK_FALSE;
    }
    
    bool checkValidationLayerSupport() {
        // Gathering some basic data of whats available
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        // Checking if all the layers in validation layers exist in the available layers list
        for (const char* layerName : validationLayers){
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
    
    void createInstance(){
        if (enableValidationLayers && !checkValidationLayerSupport()){
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
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        
        // Some extra code to let vulkan know if validation layers have been enabled or not and to pass the relevent info through
        if (enableValidationLayers){
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        // Actually make the instance now that it has its filled out checklist of every detail to ever exist
        // Also Error checking for if any of the stuff above is causing trouble
        if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
            throw std::runtime_error("Failed to create instance");
        }
    }

    void mainLoop() {
        while(!glfwWindowShouldClose(window)){
            glfwPollEvents();
        }
    }

    void cleanup() {
        // Clean up the messenger system if validation layers are enabled
        if (enableValidationLayers) {
            DestoryDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        
        // General Cleanup to free memory
        vkDestroyInstance(instance, nullptr);
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
