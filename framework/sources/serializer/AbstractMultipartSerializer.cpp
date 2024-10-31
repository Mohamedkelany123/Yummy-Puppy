#include <AbstractMultipartSerializer.h>


static inline void ltrim(std::string &s) {
    // s.erase(s.begin(), std::find_if(s.begin(), s.end(),
    //         std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c);}));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    // s.erase(std::find_if(s.rbegin(), s.rend(),
    //         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c);}).base(),s.end());

}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

MultipartField::MultipartField()
{
    value = NULL;
    value_size  =0;
}
void MultipartField::addAttribute(string _name,string _value)
{
    cout << "....... Adding Attribute: "<< _name << endl;
    std::replace( _value.begin(), _value.end(), '"', ' ');
    trim(_value); 
    attributes[_name] = _value;
}
void MultipartField::setValue(char * _value,long _value_size)
{
    value = _value;
    value_size = _value_size;
}
string MultipartField::getAttribute (string _name)
{
    if (attributes.find(_name) != attributes.end())
        return attributes[_name];
    return "";
}
char * MultipartField::getValue ()
{
    return  value;
}
long MultipartField::getValueSize()
{
    return value_size;
}
bool MultipartField::writeToAFileWithFileName()
{
    if ( getAttribute("filename") != "" )
    {
        string filename = "/home/omneya/"+getAttribute("filename");
        FILE* ff = fopen (filename.c_str(),"wb");
        if (ff == NULL) return false;
        fwrite(value,value_size,1,ff);
        fclose (ff);
        return true;
    }
    else return false;
}

bool MultipartField::writeToAFile(string _file_name)
{

    FILE* ff = fopen (_file_name.c_str(),"wb");
    fwrite(value,value_size,1,ff);
    if (ff == NULL) return false;
    fclose (ff);
    return true;

}

MultipartField::~MultipartField()
{

}

//*************************************************************************************** */
AbstractMultipartSerializer::AbstractMultipartSerializer():AbstractSerializer()
{
    

}
void AbstractMultipartSerializer::serialize_binary (char * binary_buffer,long binary_size)
{
    vector <char *> tokens;
    vector <long> tokens_size;
    char * ptr = strstr (binary_buffer,"\r\n\r\n");
    if ( ptr == NULL) return;
    else ptr = ptr+4;

    for ( int  i = 0 ; ptr != NULL && binary_buffer+binary_size-ptr >0  ; i++)
    {
        if ( strncmp (ptr,"\r\n",2) == 0) 
        {
            ptr[0] = 0;
            ptr[1] = 0;
            ptr += 2;
            tokens.push_back(ptr);
            for ( ;strncmp (ptr,tokens[0],strlen(tokens[0])) != 0 && binary_buffer+binary_size-ptr >0; )
                ptr++;
            if ( ptr[-1] == '\n' )  ptr[-1]=0;
            if ( ptr[-2] == '\r' )  ptr[-2]=0;
            long token_sz = ptr - tokens[tokens.size()-1]-2;
            tokens_size.push_back(token_sz);

        }
        else {
            tokens.push_back(ptr);
            vector <char *> next_delim;
            char * ptr1 = (char*) memchr (ptr,'\r',binary_buffer+binary_size-ptr);
            char * ptr2 = (char*) memchr (ptr,';',binary_buffer+binary_size-ptr);
            char * ptr3 = (char*) memchr (ptr,'=',binary_buffer+binary_size-ptr);
            char * ptr4 = (char*) memchr (ptr,':',binary_buffer+binary_size-ptr);

            if ( ptr1 != NULL ) next_delim.push_back(ptr1);
            if ( ptr2 != NULL ) next_delim.push_back(ptr2);
            if ( ptr3 != NULL ) next_delim.push_back(ptr3);
            if ( ptr4 != NULL ) next_delim.push_back(ptr4);
            
            sort(next_delim.begin(),next_delim.end());

            if ( next_delim.size() == 0) break;
            ptr = next_delim[0];

            long token_sz = ptr - tokens[tokens.size()-1];
            tokens_size.push_back(token_sz);

            ptr[0] = 0;
            ptr ++;
            if ( ptr[0] == '\n' || ptr[0] == ' ')
            {
                ptr[0] = 0;
                ptr ++;
            }
        }
    }

    for ( int  i = 0 ; i < tokens.size() ; i ++) {
        if (strncmp (tokens[i],tokens[0],strlen(tokens[0])) == 0 )
        {
            continue;
        }
        else  if (i+1 < tokens.size() && strncmp (tokens[i+1],tokens[0],strlen(tokens[0])) == 0 )
        {
            multipartFields[multipartFields.size()-1].setValue(tokens[i],tokens_size[i]);
        }
        else 
        {
            if ( i> 0 && strcmp (tokens[i-1],tokens[0]) == 0 )
                multipartFields.push_back(MultipartField());
            multipartFields[multipartFields.size()-1].addAttribute(tokens[i],tokens[i+1]);
            i++;
        }
    }
    for ( int i = 0 ; i < multipartFields.size() ; i ++)
        multipartFieldIndex[multipartFields[i].getAttribute("name")] = & multipartFields[i];

}
MultipartField * AbstractMultipartSerializer::getMultipartField(string _name)
{
    if (multipartFieldIndex.find(_name) != multipartFieldIndex.end())
        return multipartFieldIndex[_name];
    else return NULL;    
}
void AbstractMultipartSerializer::serialize (string ss)
{

}
string AbstractMultipartSerializer::deserialize ()
{
    return "";
}
AbstractMultipartSerializer::~AbstractMultipartSerializer()
{

}
