#include <FileReader.h>


FileReader::FileReader(string _file_name)
{
    f = fopen (_file_name.c_str(),"rb");
    if ( f != NULL )
    {
        fseek (f,0,2);
        file_size = ftell(f);
        fseek (f,0,0);
    }
}
string FileReader::readFile ()
{
    string file_content = "";
    if ( f != NULL)
    {
        char * buffer = (char *) calloc (file_size+1,sizeof(char));
        fread(buffer,1,file_size,f);
        file_content = buffer;
        free(buffer);
    }
    return file_content;
}
FileReader::~FileReader()
{
    if ( f != NULL ) fclose (f);
}