#ifndef ABSTRACTSERIALIZER_H
#define ABSTRACTSERIALIZER_H

#include <common.h>


class AbstractSerializer
{
    private:
    public:
        AbstractSerializer(){}
        virtual void serialize (string ss) = 0;
        virtual string deserialize () = 0;
        virtual ~AbstractSerializer(){}
};


#endif