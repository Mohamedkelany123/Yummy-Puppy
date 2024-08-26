#ifndef PSQLABSTARCTORMITERATOR_H
#define PSQLABSTARCTORMITERATOR_H

#include <PSQLAbstractQueryIterator.h>




template <typename T>
class PSQLAbstractORMIterator:public PSQLAbstractQueryIterator {
    private:
        static void process_internal(PSQLAbstractORMIterator * psqlAbstractQueryIterator,string data_source_name, PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extra_params,std::function<void(T * orm,int partition_number,mutex * shared_lock,void * extra_params)> f)
        {
                PSQLQueryPartitionIterator <T> psqlQueryPartitionIterator (psqlQueryPartition, data_source_name, extra_params, partition_number);
                T * orm = NULL;
                do {
                    orm =psqlQueryPartitionIterator.next();
                    if (orm != NULL) 
                    {
                        for (auto e: psqlAbstractQueryIterator->extras)
                            orm->setExtra(e.first,psqlQueryPartition->getValue(e.first));
                        f(orm,partition_number,shared_lock, extra_params);

                        shared_lock->lock();
                        orm->unlock_me();
                        shared_lock->unlock();
                    }
                } while (orm != NULL);
                psqlController.unlock_current_thread_orms(data_source_name);
        }
    public:
        PSQLAbstractORMIterator(string _data_source_name,string orm_table_name, int _partition_number):PSQLAbstractQueryIterator(_data_source_name,orm_table_name, _partition_number){
            T * orm = new T(_data_source_name);
            from_string =orm->getFromString();
            orderby_string = orm->getIdentifierName() + " asc";
            delete (orm); // This is add and need to be tested 
        }

        T * operator [] (long index)
        {
            return NULL;
        }
        T * next (bool _read_only)
        {
            if (psqlQuery->fetchNextRow())
            {
                T * obj = NULL;

                if ( _read_only)
                {
                    obj = new T(data_source_name);
                    obj->assignResults(psqlQuery,_read_only);
                }
                else
                {
                    T * seeder = new T(data_source_name);
                    obj= (T *)psqlController.addToORMCache(seeder,psqlQuery,-1,data_source_name);
                    delete (seeder);

                }
                for (auto e: extras)
                    obj->setExtra(e.first,psqlQuery->getValue(e.first));
                return obj;
            }
            else return NULL;
        }
        T * back ()
        {
            return NULL;
        }

        void process(int partitions_count,std::function<void(T * orm,int partition_number,mutex * shared_lock,void * extras)> f, void * extra_params)
        {
            time_t start = time (NULL);
            cout << "Executing PSQL Query on the remote server" << endl;
            if (this->execute() && ((PSQLQuery *)this->psqlQuery)->getRowCount() > 0)
            {
                time_t time_snapshot1 = time (NULL);

                cout << "Query results " << this->psqlQuery->getRowCount() << " in "  << (time_snapshot1-start)<< " seconds .."<<endl;
                cout << "Starting multi-threading execution" << endl;

                vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
                vector <thread *> threads;
                mutex shared_lock;
                for (size_t   i  = 0 ; i < p->size() ; i ++)
                {
                    thread * t = new thread(process_internal, this,data_source_name, (*p)[i],i,&shared_lock,extra_params,f);
                    threads.push_back(t);
                }
                for (size_t   i  = 0 ; i < p->size() ; i ++)
                {
                        thread * t = threads[i];
                        t->join();
                        delete (t);
                }

                time_t time_snapshot2 = time (NULL);
                cout << "Finished multi-threading execution" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
                cout << "cache counter: " << psqlController.getCacheCounter(data_source_name) << endl;

            }
        }

        int getRowCount(){
            return this->psqlQuery->getRowCount();
        }

        ~PSQLAbstractORMIterator ()
        {
            
        }
};
#endif
