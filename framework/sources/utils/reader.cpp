#include <reader.h>


ConfigReader* ConfigReader::instance = nullptr;
std::mutex ConfigReader::instanceMutex;