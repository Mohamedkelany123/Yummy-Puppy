#ifndef _URLSERVICE_H_
#define _URLSERVICE_H_
#include <common_ws.h>
#include <regex>

class URLService
{
public:
    /**
     * Extracts parameters enclosed within angle brackets '<' and '>' from a given URL path.
     *
     * @param URLPath A string representing the URL path from which to extract parameters.
     * @return A pointer to a vector of strings containing the extracted parameters.
     *         The caller is responsible for deallocating the memory for the vector.
     * @author Ramy
     * @date 14-Aug-2024
     */
    static vector<string> *getURLParams(string URLPath)
    {
        vector<string> *parameters = new vector<string>();
        std::string::size_type start = 0;
        while ((start = URLPath.find('<', start)) != std::string::npos)
        {
            // Loop to find all possible end positions for the substring
            auto end = start;
            if ((end = URLPath.find('>', end + 1)) != std::string::npos)
            {
                parameters->push_back(URLPath.substr(start + 1, end - start - 1));
            }
            start = end;
        }

        return parameters;
    }
    /**
     * Generates a regular expression URL pattern from a given URL path.
     * Replaces all occurrences of angle brackets '<' and '>' with '.*' to match any sequence of characters.
     *
     * @param URLPath A string representing the URL path from which to generate the regular expression pattern.
     * @return A string representing the regular expression URL pattern.
     * @author Ramy
     * @date 14-Aug-2024
     */
    static string getRegexURL(string URLPath)
    {
        std::string::size_type start, end;
        while ((start = URLPath.find('<')) != std::string::npos && (end = URLPath.find('>', start)) != std::string::npos)
        {
            URLPath.replace(start, end - start + 1, ".*");
        }
        return URLPath;
    }

    /**
     * Searches a map of regular expressions and their corresponding objects for a matching key.
     * If a match is found, the function returns a pair containing the matched key and the corresponding object.
     * If no match is found, the function returns a pair with the search key and a NULL pointer.
     *
     * @param searchkey A string representing the key to search for in the map.
     * @param regexMap A pointer to a map containing regular expressions as keys and objects of type T as values.
     * @return A pair containing the matched key (or the search key if no match is found) and a pointer to the corresponding object (or NULL).
     *
     * @tparam T The type of the objects stored in the map.
     * @author Ramy
     * @date 14-Aug-2024
     */
    template <class T>
    static pair<string, T *> searchRegexMapWithKey(string searchkey, map<string, T *> *regexMap)
    {
        if (regexMap == NULL)
            return {"", NULL};
        if (regexMap->operator[](searchkey) == NULL) // if not found
        {
            // Extract file base noame
            for (auto entry : *regexMap)
            {
                if (entry.first.find("*") != std::string::npos)
                {
                    cout << "searchkey: " << searchkey << endl;
                    cout << "entry.first: " << entry.first << endl;

                    if (std::regex_match(searchkey, std::regex(entry.first)))
                    {
                        // return pair<string, T>(entry.first, regexMap->at(entry.first)); // else clone service based on base file name
                        return entry;
                    }
                }
            }
            // TODO Make sure this is a valid return T() might cause segmentation fault if T cannot be initialized
            return {searchkey, NULL};
        }
        else
            return {searchkey, regexMap->operator[](searchkey)}; // else clone service based on base file name
    }

    /**
     * Splits a given URL into a vector of strings based on a specified split key.
     *
     * @param URL A string representing the URL to be split.
     * @param splitKey A character representing the delimiter used to split the URL.
     *                  The default value is '/' (forward slash).
     *
     * @return A vector of strings containing the split components of the URL.
     *         If the URL is empty or the split key is not found, an empty vector is returned.
     * @author Ramy
     * @date 14-Aug-2024
     */
    static vector<string> * splitURL(string URL, char splitKey = '/')
    {
        vector<string> * result = new vector<string>();
        stringstream ss(URL);
        string item;

        while (getline(ss, item, splitKey))
        {
            (*result).push_back(item);
        }
        return result;
    }
};

#endif
