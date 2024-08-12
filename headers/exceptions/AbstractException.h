#ifndef ABSTRACTEXCEPTION_H
#define ABSTRACTEXCEPTION_H

#include <AbstractSerializer.h>

class AbstractException
{
        private:
        public:
            AbstractException(){}
            virtual void handle(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer )=0;
            virtual ~AbstractException(){}
};   


#endif