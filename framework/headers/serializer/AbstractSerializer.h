#ifndef ABSTRACTSERIALIZER_H
#define ABSTRACTSERIALIZER_H

#include <common.h>

// #include <ThorSerialize/Traits.h>
// #include <ThorSerialize/SerUtil.h>
// #include <ThorSerialize/JsonThor.h>

class AbstractSerializer
{
    private:
    public:
        AbstractSerializer(){}
        virtual void serialize_binary (char * binary_buffer,long binary_size){}
        virtual void serialize (string ss) = 0;
        virtual string deserialize () = 0;
        virtual ~AbstractSerializer(){}
};


#endif