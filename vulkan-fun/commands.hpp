#ifndef commands_hpp
#define commands_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "devices.hpp"
#include "swapchain.hpp"
#include "frameBuffer.hpp"
#include "renderPass.hpp"
#include "graphicsPipeline.hpp"

class commands{
public:
    void initCommands(devices* initDevices, swapchain* initSwapchain, framebuffer* initFramebuffer, renderPass* initRenderpass, graphicsPipeline* initGraphicsPipeline);
    void destroyCommands();
private:
    void createCommandPool();
    void createCommandBuffers();
    
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    
    devices* pDevices;
    swapchain* pSwapchain;
    framebuffer* pFramebuffer;
    renderPass* pRenderpass;
    graphicsPipeline* pGraphicsPipeline;
};

#endif /* commands_hpp */
