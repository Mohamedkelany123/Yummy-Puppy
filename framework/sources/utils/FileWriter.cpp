#include <FileWriter.h>


FileWriter::FileWriter(string _file_name)
{
    f = fopen (_file_name.c_str(),"wb");
}
bool FileWriter::writeFile (string _file_content)
{
    if ( f != NULL)
    {
        fwrite(_file_content.c_str(),1,_file_content.length(),f);
        return true;
    }
    return false;
}
FileWriter::~FileWriter()
{
    if ( f != NULL ) fclose (f);
}