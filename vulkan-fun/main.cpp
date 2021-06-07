#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include "windowManager.hpp"
#include "vulkan.hpp"

//TODO
// Move some of the class starter function that take in inits to a new init function that calls everything that way
// Things can't be called out of order by accident, theres only one init function that does everything
// Looks like swapchain and devices would only need to be changed out, might break some stuff when you do it however
// But well cross that bridge when we get there

// Width and Height of the window
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

// Defines how many frames can be processed concurrently
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

//std::string test = "VK_KHR_portability_subset";
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
//    test.c_str()
};

// Fancy code to toggle validation layers when compiling a debug build
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers= true;
#endif

class HelloTriangleApplication{
public:
    void run() {
        // Overall progression of big events
        window.init(&enableValidationLayers, &WIDTH, &HEIGHT);
        vulkan.initVulkan(&enableValidationLayers, &validationLayers, &deviceExtensions, &window, &MAX_FRAMES_IN_FLIGHT);
        mainLoop();
        cleanup();
    }
private:
    // Starts out the modules
    windowManager window;
    vulkan vulkan;

    void mainLoop() {
        while(!glfwWindowShouldClose(window.window)){
            glfwPollEvents();
            vulkan.drawFrame();
        }
        
        vkDeviceWaitIdle(vulkan.devices.device);
    }

    void cleanup() {
        vulkan.destroyVulkan();
        window.destroyWindow();
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
