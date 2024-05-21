#include <%s.h>

%s


%s::%s(string _data_source_name):PSQLAbstractQueryIterator(_data_source_name,"%s"){
    %s * orm = new %s(_data_source_name);
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
        %s * obj = new %s(data_source_name);
        obj->assignResults(psqlQuery,_read_only);
        for (auto e: extras)
			obj->setExtra(e.first,psqlQuery->getValue(e.first));
        return obj;
    }
    else return NULL;
}
%s * %s::back ()
{
    return NULL;
}

void  %s::process_internal(%s * psqlAbstractQueryIterator,string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extra_params,std::function<void(%s * orm,int partition_number,mutex * shared_lock,void * extra_params)> f)
{
        PSQLQueryPartitionIterator <%s> psqlQueryPartitionIterator (psqlQueryPartition, data_source_name, extra_params);
        %s * orm = NULL;
        do {
            orm =psqlQueryPartitionIterator.next();
            if (orm != NULL) 
            {
                for (auto e: psqlAbstractQueryIterator->extras)
					orm->setExtra(e.first,psqlQueryPartition->getValue(e.first));
                f(orm,partition_number,shared_lock, extra_params);
            }
        } while (orm != NULL);
    
}

void %s::process(int partitions_count,std::function<void(%s * orm,int partition_number,mutex * shared_lock,void * extras)> f, void * extra_params)
{
    if (this->execute())
    {
        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
        vector <thread *> threads;
        mutex shared_lock;
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
            thread * t = new thread(process_internal, this,data_source_name, (*p)[i],i,&shared_lock,extra_params,f);
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

int %s::getRowCount(){
    return this->psqlQuery->getRowCount();
}

%s::~%s ()
{
    
}