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
        static void  process_internal(%s * psqlAbstractQueryIterator,string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partitions_count,mutex * shared_lock,void * extra_params,std::function<void(%s * orm,int partition_number,mutex * shared_lock,void * extra_params)> f);
    public:
        %s(string _data_source_name);
        %s * operator [] (long index);
        %s * next (bool _read_only=false);
        %s * back ();
        int getRowCount();
        void process(int partitions_count,std::function<void(%s * orm,int partition_number,mutex * shared_lock,void * extra_params)> f, void * extra_params);
        ~%s ();
};


#endif