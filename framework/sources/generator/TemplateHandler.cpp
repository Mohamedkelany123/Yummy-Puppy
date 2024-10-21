#include <TemplateHandler.h>


string TemplateHandler::fetchTemplate(const string& filename) {
    FILE *f = fopen(filename.c_str(), "rt");
    if (f == NULL) return "";

    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* template_data = (char*)calloc(filesize + 1, sizeof(char));
    fread(template_data, 1, filesize, f);
    fclose(f);

    string result(template_data);
    free(template_data);
    return result;
}

string TemplateHandler::replacePlaceholders(const string& template_str, const unordered_map<string, string>& replacements) {
    string result = template_str;
    for (const auto& pair : replacements) {
        string placeholder = "%" + pair.first + "%";
        size_t pos = result.find(placeholder);
        while (pos != string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos = result.find(placeholder, pos + pair.second.length());
        }
    }
    return result;
}

bool TemplateHandler::generateFile(const string& filename, const string& content) {
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Failed to create file: " << filename << endl;
        return false;
    }
    outfile << content;
    outfile.close();
    return true;
}

bool TemplateHandler::deleteFile(const string& filename){
    return remove(filename.c_str()) == 0;
}
