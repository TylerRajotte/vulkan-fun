#ifndef file_hpp
#define file_hpp

#include <fstream>
#include <vector>
#include <iostream>
#include <string>

class File {
public:
    static std::vector<char> readFile(const std::string& filename);
private:
};

#endif /* file_hpp */
