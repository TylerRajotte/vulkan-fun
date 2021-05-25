#include "windowManager.hpp"

void windowManager::init(const bool* INIT_ENABLEVALIDATIONLAYERS, const uint32_t* INIT_WIDTH, const uint32_t* INIT_HEIGHT){
    pEnableValidationLayers = INIT_ENABLEVALIDATIONLAYERS;
    pWIDTH = INIT_WIDTH;
    pHEIGHT = INIT_HEIGHT;
    
    initGLFW();
}

void windowManager::initGLFW(){
    // Basic GLFW Setup code
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(*pWIDTH, *pHEIGHT, "Vulkan", nullptr, nullptr);
}

std::vector<const char*> windowManager::getRequiredExtensions() {
    // Handy Function to talk with glfw and figure out what it needs from vulkan
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    
    if (*pEnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    return extensions;
}

void windowManager::destroyWindow(){
    glfwDestroyWindow(window);
    glfwTerminate();
}
