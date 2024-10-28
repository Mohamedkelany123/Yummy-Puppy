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
        T * testDataJsonNext(){
            if (m_parsed_json_results.empty()) {
                cout << "NO RESULTS FOUND FOR PATH: " << m_test_data_folder << endl;
                return nullptr;
            }
            if (m_parsed_json_index >= m_parsed_json_results["RESULTS"].size())
            {
                cout << "END OF JSON FILE REACHED" << endl;
                return nullptr;
            }

            T * orm = new T("");
            orm->deSerialize(m_parsed_json_results["RESULTS"][m_parsed_json_index][orm->getORMName()], true);

            m_parsed_json_index ++;
            // TODO: handle the deletion of the orms
            return orm;
        }
    public:
        PSQLAbstractORMIterator(string _data_source_name,string orm_table_name, int _partition_number , string _test_data_folder=""):PSQLAbstractQueryIterator(_data_source_name,orm_table_name, _partition_number, _test_data_folder){
            T * orm = new T(_data_source_name);
            from_string =orm->getFromString();
            orderby_string = orm->getIdentifierName() + " asc";
            delete (orm); // This is add and need to be tested 
        }

        T * operator [] (long index)
        {
            return NULL;
        }
        T * next (bool _read_only = false)
        {
            if(m_test_data_folder != "") 
                return testDataJsonNext();

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


        void process_from_serialized_orms(string _file_name,std::function<void(T * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras)
        {
            this->parseFile(_file_name);
            if (m_parsed_json_results.empty()) return;
            mutex shared_lock;
            for (auto jj: m_parsed_json_results["RESULTS"])
            {
                T * orm = new T("");
                orm->deSerialize(jj[orm->getORMName()], true);
                for(auto it = jj["extras"].begin(); it != jj["extras"].end(); ++it){
                    orm->setExtra(it.key(),it.value());
                }
                f(orm,partition_number,&shared_lock,extras);
                delete (orm);
            }
        }

        void serialize_results (string file_name)
        {
            if (this->execute() && this->psqlQuery->getRowCount() > 0)
            {
                string json_string  = "{\"RESULTS\":[\n";
                int counter1=0;
                for (;;)
                {
                    T * orm = next(true);
                    if (orm == NULL) break;
                    if ( counter1 > 0 )
                        json_string += ",";
                    json_string += "{\n";
                    string temp= "";
                    json_string += "\"extras\" : {";
                    map<string,string> orm_extras = orm->getExtras();
                    for(auto extra = orm_extras.begin(); extra != orm_extras.end(); ++extra){
                        json_string += "\"" + extra->first + "\":" + "\"" + extra->second + "\"" ;
                        if(std::next(extra) != orm_extras.end())
                            json_string += "\n,";
                    }
                    json_string += "},\n";
                    temp += orm->serialize();
             
                    std::replace( temp.begin(), temp.end(), '\n', ' ');
                    json_string += temp;
                    json_string += "}";
                    counter1++;
                }
                json_string +="]}";
                FileWriter * fileWriter  = new FileWriter(file_name);
                fileWriter->writeFile(json_string);
                delete (fileWriter);
            }
        }


        void process(int partitions_count,std::function<void(T * orm,int partition_number,mutex * shared_lock,void * extras)> f, void * extra_params,string test_data_file="",bool serialize=false)
        {

            if ( test_data_file  !="" && !serialize)
            {
                process_from_serialized_orms(test_data_file,f,extra_params);
                return;
            }
            else if (test_data_file  !="" && serialize)
            {
                serialize_results(test_data_file);
                return;
            }
            //TODO: Implement exception stack
            // exceptions->clear();


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

        void process(int partitions_count, void * extra_params){}

        ~PSQLAbstractORMIterator ()
        {
        }
};
#endif
