#ifndef %s
#define %s

#include <PSQLAbstractORM.h>
#include <PSQLAbstractQueryIterator.h>

%s

class %s : public PSQLAbstractORM
{
    private:
%s
    protected:
%s
    public:
%s
};

class %s : public PSQLAbstractQueryIterator
{
    public:
    %s(string _data_source_name);
    %s * operator [] (long index);
    %s * next ();
    %s * back ();
    void process(std::function<void(%s * orm)> f);
    ~%s ();
};


#endif