#include "vulkan.hpp"

void vulkan::initVulkan(const bool* initEnableValidationLayers, const std::vector<const char*>* initValidationLayers, const std::vector<const char*>* initDeviceExtensions, windowManager* initWindow, const int* initMaxFramesInFlight){
    pEnableValidationLayers = initEnableValidationLayers;
    pValidationLayers = initValidationLayers;
    pDeviceExtensions = initDeviceExtensions;
    pWindow = initWindow;
    pMaxFramesInFlight = initMaxFramesInFlight;
    
    currentFrame = 0;
    
    createInstance();
    debugMessengerUtil.setupDebugMessenger(pEnableValidationLayers, &instance);
    createSurface();
    devices.initDeviceSetup(&surface, pDeviceExtensions, pEnableValidationLayers, pValidationLayers);
    devices.pickPhysicalDevice(&instance);
    devices.createLogicalDevice();
    swapchain.createSwapChain(pWindow, &devices, &surface);
    swapchain.createImageViews();
    renderPass.createRenderPass(&devices, &swapchain);
    graphicsPipeline.createGraphicsPipeline(&devices, &swapchain, &renderPass);
    framebuffer.createFramebuffers(&devices, &swapchain, &renderPass);
    commands.initCommands(&devices, &swapchain, &framebuffer, &renderPass, &graphicsPipeline);
    createSyncObjects();
}

void vulkan::drawFrame(){
    // The big and real draw function, this will aquire an image from the swapchain, execute the command buffer wiht that image as an attachment in the framebuffer, and return the image to the swap chain for presentation
    vkWaitForFences(devices.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(devices.device, swapchain.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    
    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(devices.device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    VkCommandBuffer currentCommandBuffer = commands.commandBuffers[imageIndex];
    submitInfo.pCommandBuffers = &currentCommandBuffer;
    
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(devices.device, 1, &inFlightFences[currentFrame]);
    
    if (vkQueueSubmit(devices.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS){
        throw std::runtime_error("Failed to submit draw command buffers!");
    }
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapChains[] = {swapchain.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    vkQueuePresentKHR(devices.presentQueue, &presentInfo);
    vkQueueWaitIdle(devices.presentQueue);
    
    currentFrame = (currentFrame + 1) % *pMaxFramesInFlight;
}

bool vulkan::checkValidationLayerSupport() {
    // Gathering some basic data of whats available
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    // Checking if all the layers in validation layers exist in the available layers list
    for (const char* layerName : *pValidationLayers){
        bool layerFound = false;
        
        for (const auto& layerProperties : availableLayers){
            if (strcmp(layerName, layerProperties.layerName) == 0){
                layerFound = true;
                break;
            }
        }
        
        if (!layerFound) {
            return false;
        }
    }
    
    return true;
}

void vulkan::createInstance(){
    if (*pEnableValidationLayers && !checkValidationLayerSupport()){
        throw std::runtime_error("validation layers requested, but not available");
    }
    
    // Fill out the basic info into a special stucture object, maybe something to be moved to a settings.txt style file
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Pass the previous structure it into the createinfo structure
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    // Call the function that figures out what glfw needs to do its thing
    auto extensions = pWindow->getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    
    // Some extra code to let vulkan know if validation layers have been enabled or not and to pass the relevent info through
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (*pEnableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(pValidationLayers->size());
        createInfo.ppEnabledLayerNames = pValidationLayers->data();
        
        // Some extra stuff so that we can get debug messengers when creating the instance
        debugMessengerUtil.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        
        createInfo.pNext = nullptr;
    }

    // Actually make the instance now that it has its filled out checklist of every detail to ever exist
    // Also Error checking for if any of the stuff above is causing trouble
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        throw std::runtime_error("Failed to create instance");
    }
}

void vulkan::createSurface(){
    if(glfwCreateWindowSurface(instance, pWindow->window, nullptr, &surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to create window surface!");
    }
}

void vulkan::createSyncObjects(){
    // Sets up the semaphores so that everything can be synced even if things finish at different rates
    imageAvailableSemaphores.resize(*pMaxFramesInFlight);
    renderFinishedSemaphores.resize(*pMaxFramesInFlight);
    inFlightFences.resize(*pMaxFramesInFlight);
    imagesInFlight.resize(swapchain.swapChainImages.size(), VK_NULL_HANDLE);
    
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (size_t i = 0; i < *pMaxFramesInFlight; i++){
        if (vkCreateSemaphore(devices.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS | vkCreateSemaphore(devices.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS |
            vkCreateFence(devices.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS){
            throw std::runtime_error("failed to create sync objects for a frame!");
        }
    }
}

void vulkan::destroyVulkan(){
    // Cleanup and Free the things used
    for (size_t i = 0; i < *pMaxFramesInFlight; i++){
        vkDestroySemaphore(devices.device, renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(devices.device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(devices.device, inFlightFences[i], nullptr);
    }
    commands.destroyCommands();
    framebuffer.destroyFramebuffers();
    graphicsPipeline.destroyGraphicsPipeline();
    renderPass.destroyRenderPass();
    swapchain.destroySwapChain();
    devices.destroyDevices();
    
    // Clean up the messenger system if validation layers are enabled
    if (*pEnableValidationLayers) {
        debugMessengerUtil.destroyDebugMessengerUtil(&instance);
    }
    
    // General Cleanup to free memory
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}
