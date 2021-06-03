#ifndef frameBuffer_hpp
#define frameBuffer_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "devices.hpp"
#include "swapchain.hpp"
#include "renderPass.hpp"
#include "frameBuffer.hpp"

class framebuffer{
public:
    void createFramebuffers(devices* initDevices, swapchain* initSwapchain, renderPass* initRenderpass);
    void destroyFramebuffers();
    
    std::vector<VkFramebuffer> swapChainFramebuffers;
private:
    devices* pDevices;
    swapchain* pSwapchain;
    renderPass* pRenderpass;
};

#endif /* frameBuffer_hpp */
