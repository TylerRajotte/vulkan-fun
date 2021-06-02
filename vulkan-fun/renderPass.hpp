#ifndef renderPass_hpp
#define renderPass_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "devices.hpp"
#include "swapchain.hpp"

class renderPass{
public:
    void createRenderPass(devices* initDevices, swapchain* initSwapchain);
    void destroyRenderPass();
    
    VkRenderPass renderPass;
private:
    devices* pDevices;
    swapchain* pSwapchain;
};

#endif /* renderPass_hpp */
