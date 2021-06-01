#ifndef graphicsPipeline_hpp
#define graphicsPipeline_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "file.hpp"
#include "devices.hpp"
#include "swapchain.hpp"

class graphicsPipeline{
public:
    void createGraphicsPipeline(devices* initDevices, swapchain* initSwapchain);
    void destroyGraphicsPipeline();
private:
    File file;
    devices* pDevices;
    swapchain* pSwapchain;
    
    VkPipelineLayout pipelineLayout;
    
    VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif /* graphicsPipeline_hpp */
