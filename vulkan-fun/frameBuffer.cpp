#include "frameBuffer.hpp"

void Framebuffer::createFramebuffers(devices* initDevices, swapchain* initSwapchain, renderPass* initRenderpass){
    pDevices = initDevices;
    pSwapchain = initSwapchain;
    pRenderpass = initRenderpass;
    
    // Framebuffer reference all the VkImageView objects that represent the attachments
    // I belive this is what feeds from the swapchain into the window if im not mistaken
    swapChainFramebuffers.resize(pSwapchain->swapChainImageViews.size());
    
    for (size_t i = 0; i < pSwapchain->swapChainImageViews.size(); i++){
        VkImageView attachments[] = {
          pSwapchain->swapChainImageViews[i]
        };
        
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pRenderpass->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = pSwapchain->swapChainExtent.width;
        framebufferInfo.height = pSwapchain->swapChainExtent.height;
        framebufferInfo.layers = 1;
        
        if (vkCreateFramebuffer(pDevices->device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void Framebuffer::destroyFramebuffers(){
    for (auto framebuffer : swapChainFramebuffers){
        vkDestroyFramebuffer(pDevices->device, framebuffer, nullptr);
    }
}
