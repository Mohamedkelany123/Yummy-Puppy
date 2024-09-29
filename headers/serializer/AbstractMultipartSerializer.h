#ifndef ABSTRACTMULTIPARTSERIALIZER_H
#define ABSTRACTMULTIPARTSERIALIZER_H

#include <AbstractSerializer.h>

class AbstractMultipartSerializer: public AbstractSerializer
{
    private:
    public:
        AbstractMultipartSerializer();
        virtual void serialize_binary (char * binary_buffer,long binary_size);
        virtual void serialize (string ss);
        virtual string deserialize ();
        virtual ~AbstractMultipartSerializer();
};


#endif