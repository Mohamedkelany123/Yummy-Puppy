#ifndef ABSTRACTEXCEPTION_H
#define ABSTRACTEXCEPTION_H

#include <AbstractSerializer.h>

class AbstractException
{
        private:
        public:
            AbstractException(){}
            virtual void handleFirst(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer ){}
            virtual void handleLast(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer ){}
            virtual void push_back(AbstractException * _exception){}
            virtual AbstractException * operator [] (int index){ return NULL;}
            virtual void handle(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer )=0;
            virtual int size () {return  0;}
            virtual void clear (){}
            virtual ~AbstractException(){}
};   


class ExceptionStack:public AbstractException
{
        private:
            vector <AbstractException *> exceptionStack;
        public:
            ExceptionStack():AbstractException(){}
            virtual void handleFirst(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer )
            {
                if (exceptionStack.size() >  0 ) exceptionStack[0]->handle(inputSerializer,outputSerializer);
            }
            virtual void handleLast(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer )
            {
                if (exceptionStack.size() >  0 ) exceptionStack[exceptionStack.size()-1]->handle(inputSerializer,outputSerializer);
            }
            virtual void push_back(AbstractException * _exception){ 
                exceptionStack.push_back(_exception);
            }
            virtual AbstractException * operator [] (int index)
            {
                if (exceptionStack.size() >  index ) return exceptionStack[index];
                else return NULL;

            }
            virtual void handle(AbstractSerializer * inputSerializer, AbstractSerializer * outputSerializer )
            {
                for (int i = 0 ; i  < exceptionStack.size() ; i++)
                    exceptionStack[i]->handle(inputSerializer,outputSerializer);
            }
            virtual int size ()
            {
                    return exceptionStack.size();
            }
            virtual void clear()
            {
                for (int i = 0 ; i  < exceptionStack.size() ; i++)
                    delete (exceptionStack[i]);
                exceptionStack.clear();
            }
            virtual ~ExceptionStack(){
                clear();
            }
};   

class BadRequest : public exception {
private:
    string message;

public:

    BadRequest(const char* msg)
        : message(msg)
    {
    }

    const char* what() const throw()
    {
        return message.c_str();
    }
    string getMessege(){
        return message;
    }
};

#endif