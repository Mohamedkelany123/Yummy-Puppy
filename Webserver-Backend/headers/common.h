#ifndef COMMON_H_
#define COMMON_H_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <typeinfo>
#include <cstdint>
#include <array>
#include <iterator>
#include <map>
#include <sstream>
#include <cstdlib>
#include <sstream>
#include <strings.h>
#include <thread>
#include <mutex>
#include <exception>
#include <json.hpp>
using namespace std;
using json = nlohmann::json;

struct ignorecase {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};
#endif // COMMON_H_
