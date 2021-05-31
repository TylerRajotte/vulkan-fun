#include "swapchain.hpp"

VkSurfaceFormatKHR swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats){
    //Tries to grab the some good default color channels and types if there not around just go with the first next best one
    for (const auto& availableFormat : availableFormats){
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

VkPresentModeKHR swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes){
    // VSync Settings right here, tries for triple buffering  if not tries the standard mode of swapchain doing
    for (const auto& availablePresentMode : availablePresentModes){
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR){
            return availablePresentMode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
    // Makes sure that the proper screen coords or screen size is being used depending on whats needed and it converts between the two
    // Also makes sure that everything is going to actually work with vulkan and isnt out of spec
    if (capabilities.currentExtent.width != UINT32_MAX){
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(pWindow->window, &width, &height);
        
        VkExtent2D actualExent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        
        actualExent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExent.width));
        actualExent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExent.height));
        
        return actualExent;
    }
}

void swapchain::createSwapChain(windowManager* initWindow, devices* initDevices, VkSurfaceKHR* initSurface){
    pWindow = initWindow;
    pDevices = initDevices;
    pSurface = initSurface;
    
    // Figure out what we can and can't do as a starting point
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(&pDevices->physicalDevice, pSurface);
    
    // Gathers info for filling another data struct "Vulkan Form" trieds to grabs whats the reasonable defaults but will go back to what works
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    
    // Sets up however many images were going to store in the swap chain generally 1 plus the minium
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount){
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *pSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndices indices = pDevices->findQueueFamilies(pDevices->physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    
    if (indices.graphicsFamily != indices.presentFamily){
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    if (vkCreateSwapchainKHR(pDevices->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS){
        throw std::runtime_error("Failed to create swapchain");
    }
    
    vkGetSwapchainImagesKHR(pDevices->device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(pDevices->device, swapChain, &imageCount, swapChainImages.data());
    
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void swapchain::createImageViews(){
    // Needed so the swapchain can actually function, creates a image view for every image in the swapchain
    swapChainImageViews.resize(swapChainImages.size());
    
    for (size_t i = 0; i < swapChainImages.size(); i++){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        if(vkCreateImageView(pDevices->device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create image views!");
        }
    }
}

void swapchain::destroySwapChain(){
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(pDevices->device, imageView, nullptr);
    }
    
    vkDestroySwapchainKHR(pDevices->device, swapChain, nullptr);
}
