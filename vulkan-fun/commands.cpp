#include "commands.hpp"

void commands::initCommands(devices* initDevices, swapchain* initSwapchain, framebuffer* initFramebuffer, renderPass* initRenderpass, graphicsPipeline* initGraphicsPipeline){
    pDevices = initDevices;
    pSwapchain = initSwapchain;
    pFramebuffer = initFramebuffer;
    pRenderpass = initRenderpass;
    pGraphicsPipeline = initGraphicsPipeline;
    
    createCommandPool();
    createCommandBuffers();
}

void commands::createCommandPool(){
    // Creates the object that stores all of the command buffer objects
    QueueFamilyIndices queueFamilyIndices = pDevices->findQueueFamilies(pDevices->physicalDevice);
    
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    if (vkCreateCommandPool(pDevices->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create command pool!");
    }
}

void commands::createCommandBuffers(){
    // I believe that each command buffer is for each frame becuase each frame contains multiple draw calls for each tri
    // So this double nested container thing does make some sense I guess
    // Uses the framebuffer to determine the size that is needed for the command buffer
    commandBuffers.resize(pFramebuffer->swapChainFramebuffers.size());
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
    
    if (vkAllocateCommandBuffers(pDevices->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
    
    for (size_t i = 0; i < commandBuffers.size(); i++){
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
            throw std::runtime_error("Failed to being recording command buffer");
        }
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pRenderpass->renderPass;
        renderPassInfo.framebuffer = pFramebuffer->swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = pSwapchain->swapChainExtent;
        
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->graphicsPipeline);
        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to record command buffer!");
        }
    }
}

void commands::destroyCommands(){
    vkDestroyCommandPool(pDevices->device, commandPool, nullptr);
}
