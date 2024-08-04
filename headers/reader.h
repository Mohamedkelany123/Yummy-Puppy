#ifndef READER_H
#define READER_H

#include <common.h>

class ConfigReader {

    private:
        string filename;
        json filedata;

        static json readConfigFile(string _filename){
            std::ifstream f(_filename);
            json json_data = json::parse(f);
            return json_data;
        }

    public:
        ConfigReader(string _filename) {
            this->filename = _filename;
            this->filedata = ConfigReader::readConfigFile(filename);
        }

        template <typename... Args>
        json GetValue(const string& first, const Args&... args) {
            json current = this->filedata;

            initializer_list<string> keys = {first, args...};
            for (const auto& key : keys) {
                if (current.contains(key)) {
                    current = current[key];
                    continue;
                }
                return NULL;
            }

            return current;
        }

        ~ConfigReader(){}

};


#endif