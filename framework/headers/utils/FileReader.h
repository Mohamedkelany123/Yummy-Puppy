#ifndef FILEREADER_H
#define FILEREADER_H

#include <common.h>

class FileReader
{
    private:
        FILE * f;
        long file_size;
    public:
        FileReader(string _file_name);
        string readFile ();
        ~FileReader();
};

#endif