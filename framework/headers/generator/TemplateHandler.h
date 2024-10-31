#ifndef TEMPLATE_HANDLER_H
#define TEMPLATE_HANDLER_H

#include <common.h>

class TemplateHandler {
public:
    static string fetchTemplate(const string& filename);
    static string replacePlaceholders(const string& template_str, const unordered_map<string, string>& replacements);
    static bool generateFile(const string& filename, const string& content);
    static bool deleteFile(const string& filename);
};

#endif
