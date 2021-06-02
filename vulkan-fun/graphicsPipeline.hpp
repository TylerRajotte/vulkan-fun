#ifndef graphicsPipeline_hpp
#define graphicsPipeline_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "file.hpp"
#include "devices.hpp"
#include "swapchain.hpp"
#include "renderPass.hpp"

class graphicsPipeline{
public:
    void createGraphicsPipeline(devices* initDevices, swapchain* initSwapchain, renderPass* initRenderpass);
    void destroyGraphicsPipeline();
    
    VkPipeline graphicsPipeline;
private:
    File file;
    devices* pDevices;
    swapchain* pSwapchain;
    renderPass* pRenderpass;
    
    VkPipelineLayout pipelineLayout;
    
    VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif /* graphicsPipeline_hpp */
