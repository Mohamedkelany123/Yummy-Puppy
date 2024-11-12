#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <map>
#include <string>
#include <iostream>

inline std::string getTestFilePath(const std::map<std::string, std::string>& testPaths, const std::string& key) {
    auto it = testPaths.find(key);
    if (it != testPaths.end()) {
        std::cout << "Test File Path[" << it->first << "]: " << it->second << std::endl;
        return it->second;
    }
    std::cout << "TEST FILE NOT FOUND FOR KEY: [" << key << "]" << std::endl;
    return "";  
}

#endif 
