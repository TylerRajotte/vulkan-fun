#ifndef windowManager_hpp
#define windowManager_hpp

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class windowManager {
public:
    void init(const bool* INIT_ENABLEVALIDATIONLAYERS, const uint32_t* INIT_WIDTH, const uint32_t* INIT_HEIGHT);
    
    GLFWwindow* window;
    std::vector<const char*> getRequiredExtensions();
    void destroyWindow();
    
private:
    const uint32_t* pWIDTH;
    const uint32_t* pHEIGHT;
    const bool* pEnableValidationLayers;
    
    void initGLFW();
};

#endif /* windowManager_hpp */
