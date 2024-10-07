#ifndef ABSTRACTMULTIPARTSERIALIZER_H
#define ABSTRACTMULTIPARTSERIALIZER_H

#include <AbstractSerializer.h>


class MultipartField
{
    private:
        map <string,string> attributes;
        char * value;
        long value_size;
    public:
        MultipartField();
        void addAttribute(string _name,string _value);
        void setValue(char * _value,long _value_size);
        string getAttribute (string _name);
        char * getValue ();
        long getValueSize();
        bool writeToAFileWithFileName();
        ~MultipartField();
};

class AbstractMultipartSerializer: public AbstractSerializer
{
    private:
        vector <MultipartField> multipartFields;
    public:
        AbstractMultipartSerializer();
        virtual void serialize_binary (char * binary_buffer,long binary_size);
        virtual void serialize (string ss);
        virtual string deserialize ();
        virtual ~AbstractMultipartSerializer();
};


#endif