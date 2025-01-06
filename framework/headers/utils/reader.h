#ifndef READER_H
#define READER_H

#include <common.h>
#include <memory>
#include <mutex>

class ConfigReader {
private:
    string filename;
    json filedata;

    // Private constructor
    ConfigReader(const string& _filename) {
        this->filename = _filename;
        this->filedata = ConfigReader::readConfigFile(filename);
    }

    // Static method to read the config file
    static json readConfigFile(const string& _filename) {
        std::ifstream f(_filename);
        if (!f.is_open()) {
            throw std::runtime_error("Failed to open file: " + _filename);
        }
        json json_data = json::parse(f);
        return json_data;
    }

    // Static instance of the class
    static ConfigReader* instance;
    static std::mutex instanceMutex;

public:
    ConfigReader(const ConfigReader&) = delete;
    ConfigReader& operator=(const ConfigReader&) = delete;

    // Initialize the singleton instance with a file
    static void initialize(const string& _filename) {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (!instance) {
            instance = new ConfigReader(_filename);
        } else {
            throw std::runtime_error("ConfigReader has already been initialized.");
        }
    }

    // Static method to access the singleton instance
    static ConfigReader& getInstance() {
        if (!instance) {
            throw std::runtime_error("ConfigReader has not been initialized.");
        }
        return *instance;
    }

    template <typename... Args>
    json GetValue(const string& first, const Args&... args) const {
        json current = this->filedata;

        initializer_list<string> keys = {first, args...};
        for (const auto& key : keys) {
            if (current.contains(key)) {
                current = current[key];
                continue;
            }
            throw std::invalid_argument("Key not found: " + key);
        }

        return current;
    }

    ~ConfigReader() {
        delete instance;
    }
};



#endif
