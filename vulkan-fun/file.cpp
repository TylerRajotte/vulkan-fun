#include "file.hpp"

std::vector<char> File::readFile(const std::string& filename){
    //Basic File Reader
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()){
        throw std::runtime_error("Failed to open file!");
    }
    
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    
    return buffer;
}
