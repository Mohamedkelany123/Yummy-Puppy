#include <PSQLAbstractQueryIterator.h>
#include <PSQLController.h>

PSQLAbstractQueryIterator::PSQLAbstractQueryIterator(string _data_source_name,string _table_name)
{
    data_source_name = _data_source_name;
    table_name = _table_name;
    psqlConnection = psqlController.getPSQLConnection(data_source_name);
    // sql = "select * from "+_table_name;
    from_string = " * ";
    conditions = "";
    orderby_string = "";
    sql = "";
    psqlQuery= NULL;
}
void PSQLAbstractQueryIterator::setNativeSQL(string _sql)
{
    sql=_sql;
}
void PSQLAbstractQueryIterator::filter (const Expression & e,bool print_sql)
{
    if ( conditions == "")
        conditions = " where " + e.generate();
    else conditions += " and "+ e.generate();
    if ( print_sql)
    {
        if (orderby_string == "")
            sql = "select "+ distinct +" "+from_string+" from "+ table_name + conditions ;//+" order by loan_app_loan.id";
        else sql = "select "+ distinct +" "+from_string+" from "+ table_name + conditions +" order by "+orderby_string;
        cout << sql << endl;
    }
}
bool PSQLAbstractQueryIterator::execute()
{
    if (psqlConnection == NULL ) return false;

    // cout << "EXTRAS SIZEEE ---->>>" <<  extras.size() << endl;
    for (auto e : extras)
        from_string += "," +e.second+" \""+e.first+"\"";

    string select_stmt = "";
    if (distinct != "") {
        select_stmt = distinct;
    } else {
        select_stmt = from_string;
    }

    if (orderby_string == "")
        sql = "select "+ select_stmt+" from "+ table_name + conditions ;//+" order by loan_app_loan.id";
    else sql = "select "+ select_stmt+" from "+ table_name + conditions +" order by "+orderby_string;
    
    // cout << sql << endl;
    
    psqlQuery = psqlConnection->executeQuery(sql);
    if (psqlQuery != NULL) return true;
    else return false;
}

long PSQLAbstractQueryIterator::getResultCount()
{
        if (psqlQuery ==NULL) return 0;
        else return psqlQuery->getRowCount();
}

void PSQLAbstractQueryIterator::setOrderBy(string _orderby_string)
{
    orderby_string= _orderby_string;
}

void PSQLAbstractQueryIterator::setDistinctString(string _distinct_string)
{
    distinct= _distinct_string;
}

void PSQLAbstractQueryIterator::addExtraFromField (string field, string field_name)
{
    extras[field_name] = field;
}

PSQLAbstractQueryIterator::~PSQLAbstractQueryIterator()
{
    psqlController.releaseConnection(data_source_name,psqlConnection);
    if ( psqlQuery != NULL) delete (psqlQuery);
}


PSQLJoinQueryIterator::PSQLJoinQueryIterator(string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields):
PSQLAbstractQueryIterator(_data_source_name,"")
{
    aggregate_flag = false;
    orm_objects = new vector <PSQLAbstractORM *> ();
    for ( int i =0 ; i  < tables.size() ; i ++)
    {
        if ( column_names != "") column_names += ",";
        column_names += tables[i]->getFromString();
        if ( table_name != "") table_name += ",";
        table_name += tables[i]->getTableName();
        orm_objects->push_back(tables[i]);
        orm_objects_map[tables[i]->getORMName()]=tables[i];
    }
    for ( int i = 0 ; i < join_fields.size() ; i ++)
    {
        // cout << join_fields[i].first.first << endl;
        // cout << join_fields[i].first.second << endl;
        // cout << join_fields[i].second.first << endl;
        // cout << join_fields[i].second.second << endl;
        if ( join_string != "") join_string += " and ";
        join_string += "\""+join_fields[i].first.first+"\"."+
        "\""+join_fields[i].first.second+"\" = "+
        "\""+join_fields[i].second.first+"\"."+
        "\""+join_fields[i].second.second+"\"";
    }

    from_string = column_names;
    conditions = " where "+join_string;
}
map <string,PSQLAbstractORM *> * PSQLJoinQueryIterator::next (bool read_only)
{
    if (psqlQuery->fetchNextRow())
    {
        map <string,PSQLAbstractORM *> * results  = new map <string,PSQLAbstractORM *>();
        for (auto orm_object: *orm_objects) 
        {
            PSQLAbstractORM * orm = orm_object->clone();
            orm->assignResults(psqlQuery, read_only);
            (*results)[orm->getTableName()] = orm;
        }

        if (extras.size() > 0)
        {
            PSQLGeneric_primitive_orm * orm = new PSQLGeneric_primitive_orm(data_source_name);
            for (auto e : extras)
                orm->add(e.first,psqlQuery->getValue(e.first));
            (*results)["PSQLGeneric"] = orm;            
        }

        return results;
    }
    else return NULL;
}

void PSQLJoinQueryIterator::unlock_orms (map <string,PSQLAbstractORM *> *  orms)
{
    for (auto orm_pair: (*orms))
        if (orm_pair.second != NULL)
            orm_pair.second->unlock_me(true);

}
void PSQLJoinQueryIterator::adjust_orms_list (vector<map <string,PSQLAbstractORM *> *> * orms_list)
{
    for ( int i = orms_list->size()-1 ; i >= 0 ;i--)
    {
        map <string,PSQLAbstractORM *> * m = (*orms_list)[i];
        for ( auto ag : aggregate_map)
            (*m)[ag.first] = (*((*orms_list)[orms_list->size()-1])) [ag.first];
    }

}
void  PSQLJoinQueryIterator::process_internal_aggregate(string data_source_name, PSQLJoinQueryIterator * me,PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extras,std::function<void(vector<map <string,PSQLAbstractORM *> * > * orms_list,int partition_number,mutex * shared_lock,void * extras)> f)
{
        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryPartitionIterator psqlJoinQueryPartitionIterator (psqlQueryPartition,me->orm_objects,me->extras);
        map <string,PSQLAbstractORM *> *  orms = NULL;
        vector<map <string,PSQLAbstractORM *> *> *  orms_list = NULL;
        orms_list = new vector<map <string,PSQLAbstractORM *> *> ();
        string aggregate = "";
        bool finished = false;
        do {
            // do {

                if (aggregate == "")
                   aggregate =  psqlJoinQueryPartitionIterator.exploreNextAggregate();
                // if ( orms!= NULL) delete(orms);
                if (aggregate == psqlJoinQueryPartitionIterator.exploreNextAggregate())
                {
                    orms = psqlJoinQueryPartitionIterator.next();
                    orms_list->push_back(orms);
                }
                else
                {
                    me->adjust_orms_list(orms_list);
                    f(orms_list,partition_number,shared_lock,extras);
                    shared_lock->lock();
                    for (auto o : *orms_list)
                    {
                        me->unlock_orms(o);
                        PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,o);
                        if (gorm != NULL) delete (gorm);
                        delete (o);
                    }
                    shared_lock->unlock();
                    aggregate =  psqlJoinQueryPartitionIterator.exploreNextAggregate();
                    orms_list->clear();
                }
        } while (aggregate != "");
        delete (orms_list);
        // shared_lock->lock();
        // cout << "Exiting process_internal" << endl;
        // me->unlock_orms(orms);
        // cout << "Start freeing relative resources" << endl;
        psqlController.unlock_current_thread_orms(data_source_name);
        // cout << "After psqlController.unlock_current_thread_orms()" << endl;

        // Stop measuring time and calculate the elapsed time
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("THREADTIME  Step-> %.3f seconds.\n", elapsed.count() * 1e-9);
}


void  PSQLJoinQueryIterator::process_internal(string data_source_name, PSQLJoinQueryIterator * me,PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,void * extras,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras)> f)
{
        auto begin = std::chrono::high_resolution_clock::now();

        PSQLJoinQueryPartitionIterator psqlJoinQueryPartitionIterator (psqlQueryPartition,me->orm_objects,me->extras);
        map <string,PSQLAbstractORM *> *  orms = NULL;
        string aggregate = "";
        bool finished = false;
        do {
            if ( orms!= NULL) delete(orms);
            orms = psqlJoinQueryPartitionIterator.next();

            if (orms != NULL) 
            {
                f(orms,partition_number,shared_lock,extras);
                shared_lock->lock();
                me->unlock_orms(orms);
                PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,orms);
                if (gorm != NULL) delete (gorm);
               shared_lock->unlock();
            }
        } while (orms != NULL && !finished);
        psqlController.unlock_current_thread_orms(data_source_name);

        // Stop measuring time and calculate the elapsed time
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

        printf("THREADTIME  Step-> %.3f seconds.\n", elapsed.count() * 1e-9);
}

void PSQLJoinQueryIterator::process(int partitions_count,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras)
{
    time_t start = time (NULL);
    cout << "Executing PSQL Query on the remote server" << endl;
    if (this->execute() && this->psqlQuery->getRowCount() > 0)
    {
        time_t time_snapshot1 = time (NULL);

        cout << "Query results " << this->psqlQuery->getRowCount() << " in "  << (time_snapshot1-start)<< " seconds .."<<endl;
        cout << "Starting multi-threading execution" << endl;

        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
        vector <thread *> threads;
        mutex shared_lock;
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
            // cout << "----------------------In for LOOP-----------------------:"<< data_source_name << (*p)[i]<< &shared_lock<< endl;
            thread * t = NULL;
            /* if (aggregate_flag)
                t = new thread(process_internal_aggregate,data_source_name,this,(*p)[i],i,&shared_lock,extras,f);
            else  */ t = new thread(process_internal,data_source_name,this,(*p)[i],i,&shared_lock,extras,f);
            threads.push_back(t);
        }
        cout << "After Threads Creation" << endl;

        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                t->join();
                delete (t);
                delete((*p)[i]);
        }
        time_t time_snapshot2 = time (NULL);
        cout << "Finished multi-threading execution" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
    }
}

void PSQLJoinQueryIterator::process_aggregate(int partitions_count,std::function<void(vector<map <string,PSQLAbstractORM *> *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras)
{
    time_t start = time (NULL);
    cout << "Executing PSQL Query on the remote server" << endl;
    if (this->execute() && this->psqlQuery->getRowCount() > 0)
    {
        time_t time_snapshot1 = time (NULL);

        cout << "Query results " << this->psqlQuery->getRowCount() << " in "  << (time_snapshot1-start)<< " seconds .."<<endl;
        cout << "Starting multi-threading execution" << endl;

        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
        // for ( int i  = 0 ; i < p->size() ; i ++)
        //     (*p)[i]->dump();
        int start_index = 0;
        if ( p->size() > 1)
            for ( int i  = 0 ; i < p->size() ; i ++)
                start_index = (*p)[i]->adjust_for_aggregation(start_index);
        // cout << endl << endl;
        // for ( int i  = 0 ; i < p->size() ; i ++)
        //     (*p)[i]->dump();
        // exit(1);
        vector <thread *> threads;
        mutex shared_lock;
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
            // cout << "----------------------In for LOOP-----------------------:"<< data_source_name << (*p)[i]<< &shared_lock<< endl;
            thread * t = NULL;
            if (aggregate_flag)
                t = new thread(process_internal_aggregate,data_source_name,this,(*p)[i],i,&shared_lock,extras,f);
            //else  t = new thread(process_internal,data_source_name,this,(*p)[i],i,&shared_lock,extras,f);
            threads.push_back(t);
        }
        cout << "After Threads Creation" << endl;

        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                t->join();
                delete (t);
                delete((*p)[i]);
        }
        time_t time_snapshot2 = time (NULL);
        cout << "Finished multi-threading execution" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
    }
}
bool PSQLJoinQueryIterator::setDistinct (map <string,string> distinct_map)
{
    int count = 0;
    for (auto dm : distinct_map)
    {
        if (orm_objects_map.find(dm.first+"_primitive_orm") != orm_objects_map.end())
        {
            cout << orm_objects_map[dm.first+"_primitive_orm"]->compose_field_and_alias(dm.second) << endl;
            if (count == 0 )
                distinct = "distinct ";
            else distinct += ",";
            distinct+= orm_objects_map[dm.first+"_primitive_orm"]->compose_field_and_alias(dm.second);
            count ++;
        }
    }
    if ( count > 0 ) return true;
    else return false;
}

bool PSQLJoinQueryIterator::setAggregates (map <string,string> _aggregate_map)
{
    aggregate_map = _aggregate_map;
    int count = 0;
    string aggregate = "";
    aggregate_flag=false;
    for (auto agg : aggregate_map)
    {
        if (orm_objects_map.find(agg.first+"_primitive_orm") != orm_objects_map.end())
        {
            cout << orm_objects_map[agg.first+"_primitive_orm"]->compose_field_and_alias(agg.second) << endl;
            if (count == 0 )
                aggregate = "concat(";
            else aggregate += ",";
            aggregate+= "lpad("+orm_objects_map[agg.first+"_primitive_orm"]->compose_field(agg.second)+"::varchar,10,'0')";
            count ++;
        }
    }
    if (count > 0)
    {
        aggregate += ")";
        addExtraFromField(aggregate,"aggregate");
        aggregate_flag = true;
        return true;
    }
    else return false;
}

PSQLJoinQueryIterator::~PSQLJoinQueryIterator()
{
    for (auto orm_object: *orm_objects) 
        delete (orm_object);
    delete (orm_objects);

}