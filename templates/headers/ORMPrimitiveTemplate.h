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
    private:
        static void  process_internal(string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partitions_count,mutex * shared_lock,std::function<void(%s * orm,int partition_number,mutex * shared_lock)> f);
    public:
        %s(string _data_source_name);
        %s * operator [] (long index);
        %s * next (bool _read_only=false);
        %s * back ();
        void process(int partitions_count,std::function<void(%s * orm,int partition_number,mutex * shared_lock)> f);
        ~%s ();
};


#endif