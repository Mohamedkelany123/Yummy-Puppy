#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <common.h>

class FileWriter
{
    private:
        FILE * f;
        long file_size;
    public:
        FileWriter(string _file_name);
        bool writeFile (string _file_content);
        ~FileWriter();
};

#endif