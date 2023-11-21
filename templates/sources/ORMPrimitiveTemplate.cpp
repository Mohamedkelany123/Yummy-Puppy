#include <%s.h>

%s


%s::%s(string _data_source_name):PSQLAbstractQueryIterator(_data_source_name,"%s"){
    %s * orm = new %s();
    from_string =orm->getFromString();
    orderby_string = orm->getIdentifierName() + " asc";
}
%s * %s::operator [] (long index)
{
    return NULL;
}
%s * %s::next (bool _read_only)
{
    if (psqlQuery->fetchNextRow())
    {
        %s * obj = new %s();
        obj->assignResults(psqlQuery,_read_only);
        return obj;
    }
    else return NULL;
}
%s * %s::back ()
{
    return NULL;
}

void  %s::process_internal(PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,std::function<void(%s * orm,int partition_number,mutex * shared_lock)> f)
{
        PSQLQueryPartitionIterator <%s> psqlQueryPartitionIterator (psqlQueryPartition);
        %s * orm = NULL;
        do {
            orm =psqlQueryPartitionIterator.next();
            if (orm != NULL) 
            {
                f(orm,partition_number,shared_lock);
            }
        } while (orm != NULL);
    
}

void %s::process(int partitions_count,std::function<void(%s * orm,int partition_number,mutex * shared_lock)> f)
{
    if (this->execute())
    {
        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
        vector <thread *> threads;
        mutex shared_lock;
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
            thread * t = new thread(process_internal,(*p)[i],i,&shared_lock,f);
            threads.push_back(t);
        }
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                t->join();
                delete (t);
        }
    }
}


%s::~%s ()
{
    
}