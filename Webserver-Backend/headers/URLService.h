#ifndef _URLSERVICE_H_
#define _URLSERVICE_H_
#include <common_ws.h>

class URLService
{
    public:
    static vector<string> getURLParams(string URLPath) {
            vector<string> parameters;
            std::string::size_type start = 0;
            while ((start = URLPath.find('<', start)) != std::string::npos) {
                // Loop to find all possible end positions for the substring
                auto end = start;
                if ((end = URLPath.find('>', end + 1)) != std::string::npos) {
                    parameters.push_back(URLPath.substr(start+1, end - start-1));
                }
                start = end; 
            }

        return parameters;
    }
    static string getRegexURL(string URLPath)
    {       
        std::string::size_type start, end;
        while ((start = URLPath.find('<')) != std::string::npos && (end = URLPath.find('>', start)) != std::string::npos) {
            URLPath.replace(start, end - start + 1, ".*");
        }
        return URLPath;
    }

    template<class T>
    static pair<string, T> searchRegexMapWithKey(const string searchkey, map<string, T> const * regexMap){
        if ( regexMap[searchkey]==NULL)  // if not found
        {
            // Extract file base noame
            for (auto entry : regexMap){
                if (entry.first.find("*")!= std::string::npos){
                    if (std::regex_match(searchkey, std::regex(entry.first))) {
                        return {entry.first, services[entry.first]}; // else clone service based on base file name
                    }
                }
            }
            // TODO Make sure this is a valid return T() might cause segmentation fault if T cannot be initaillized 
            return {searchkey, T()};
        }
        else return {searchkey, regexMap[searchkey]}; // else clone service based on base file name
    }

    static vector<string> splitURL(string URL, char splitKey='/'){
        vector<std::string> result;
        stringstream ss(URL);
        string item;

        while (getline(ss, item, splitKey)) {
            result.push_back(item);
        }
        return result;
    }

};


#endif
