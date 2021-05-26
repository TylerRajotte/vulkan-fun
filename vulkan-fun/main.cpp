#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include "windowManager.hpp"
#include "vulkan.hpp"

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

class HelloTriangleApplication{
public:
    void run() {
        // Overall progression of big events
        window.init(&enableValidationLayers, &WIDTH, &HEIGHT);
        vulkan.initVulkan(&enableValidationLayers, &validationLayers, &window);
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
        }
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
