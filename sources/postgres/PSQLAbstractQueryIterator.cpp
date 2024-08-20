#include <PSQLAbstractQueryIterator.h>
#include <PSQLController.h>

PSQLAbstractQueryIterator::PSQLAbstractQueryIterator(string _data_source_name,string _table_name, int _partition_number)
{
    data_source_name = _data_source_name;
    table_name = _table_name;
    psqlConnection = psqlController.getPSQLConnection(data_source_name);
    // sql = "select * from "+_table_name;
    from_string = " * ";
    conditions = "";
    pre_conditions = "";
    orderby_string = "";
    sql = "";
    psqlQuery= NULL;
    partition_number = _partition_number;

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
            sql = "select "+ distinct +" "+from_string+" from "+ table_name + pre_conditions +conditions ;//+" order by loan_app_loan.id";
        else sql = "select "+ distinct +" "+from_string+" from "+ table_name + pre_conditions + conditions +" order by "+orderby_string;
        cout << sql << endl;
    }
}
bool PSQLAbstractQueryIterator::execute()
{
    if (psqlConnection == NULL ) return false;
    
    string extra_from;
    for (auto e : extras)
        extra_from += "," +e.second+" \""+e.first+"\"";

    string select_stmt = "";
    if (distinct != "") {
        select_stmt = distinct;
        
    } else {
        select_stmt = from_string;
    }

    select_stmt += extra_from;

    if (orderby_string == "")
        sql = "select "+ select_stmt+" from "+ table_name + pre_conditions+ conditions ;//+" order by loan_app_loan.id";
    else sql = "select "+ select_stmt+" from "+ table_name + pre_conditions+ conditions +" order by "+orderby_string;
    
    cout << sql << endl;
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
    conditions= "";
    pre_conditions = "";
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

JOIN_TYPE get_join_type (string table_name, vector <pair<pair<pair<string,string>,pair<string,string>>,JOIN_TYPE>> const & join_fields)
{

    for ( int  i = 0 ; i < join_fields.size() ; i++ )
    {
        if (join_fields[i].first.second.first == table_name) return join_fields[i].second;

    }
    return none;

}

string get_aux_condition (string table_name, vector <pair<pair<pair<string,string>,pair<string,string>>,JOIN_TYPE>> const & join_fields)
{
    string join_condition = "";
    for ( int  i = 0 ; i < join_fields.size() ; i++ )
    {
        if (join_fields[i].first.first.first == table_name && join_fields[i].second == JOIN_TYPE::aux) 
        {
            join_condition += " and ";
            join_condition += "\""+join_fields[i].first.first.first+"\"."+
            "\""+join_fields[i].first.first.second+"\" = ";
            if (join_fields[i].first.second.first != "" && join_fields[i].first.second.first !="<<expression>>") join_condition += "\""+join_fields[i].first.second.first+"\".";
            if (join_fields[i].first.second.first =="<<expression>>") 
                join_condition += join_fields[i].first.second.second;
            else join_condition += "\""+join_fields[i].first.second.second+"\"";
        }
    }
    return join_condition;
}
/*


select  tms_app_loaninstallmentfundingrequest.funding_facility_id as bond_id ,d.id as ins_id, d.loan_id
from loan_app_loan lal 
left join tms_app_loaninstallmentfundingrequest on tms_app_loaninstallmentfundingrequest.loan_id =lal.id 

right join 
(select lai.loan_id, lai.id from 
loan_app_installment lai 
inner join new_lms_installmentextension nli on nli.installment_ptr_id =lai.id ) as d on d.loan_id=lal.id and tms_app_loaninstallmentfundingrequest.installment_id=d.id order by d.id

select  tms_app_loaninstallmentfundingrequest.funding_facility_id as bond_id ,loan_app_installment.id as ins_id, loan_app_installment.loan_id
from loan_app_loan lal 
left join tms_app_loaninstallmentfundingrequest on tms_app_loaninstallmentfundingrequest.loan_id =lal.id 
right join loan_app_installment on loan_app_installment.loan_id = lal.id and tms_app_loaninstallmentfundingrequest.installment_id=loan_app_installment.id
inner join new_lms_installmentextension on new_lms_installmentextension.installment_ptr_id=loan_app_installment.id
where  (loan_app_installment.loan_id <= '100')
order by loan_app_installment.id asc


select  tms_app_loaninstallmentfundingrequest.funding_facility_id as bond_id ,loan_app_installment.id as ins_id, loan_app_installment.loan_id
from loan_app_loan lal 
left join tms_app_loaninstallmentfundingrequest on tms_app_loaninstallmentfundingrequest.loan_id =lal.id 
right join loan_app_installment on loan_app_installment.loan_id = lal.id 
inner join new_lms_installmentextension on new_lms_installmentextension.installment_ptr_id=loan_app_installment.id
where  tms_app_loaninstallmentfundingrequest.installment_id = loan_app_installment.id
order by loan_app_installment.id asc


*/
PSQLJoinQueryIterator::PSQLJoinQueryIterator(string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<pair<string,string>,pair<string,string>>,JOIN_TYPE>> const & join_fields) :
PSQLAbstractQueryIterator(_data_source_name,"")
{
    aggregate_flag = false;
    string other_join_string="";
    join_string = "";
    conditions= "";
    pre_conditions = "";
    orm_objects = new vector <PSQLAbstractORM *> ();
    std::set<string> tables_set;  
    for ( int i =0 ; i  < tables.size() ; i ++)
    {
        if ( tables_set.find(tables[i]->getTableName()) == tables_set.end())
        {
            tables_set.insert(tables[i]->getTableName());
            if ( column_names != "") column_names += ",";
            column_names += tables[i]->getFromString();
            JOIN_TYPE j = get_join_type(tables[i]->getTableName(),join_fields);
            if (j == none || j==full)
            {
                if ( table_name != "") table_name += ",";
                table_name += tables[i]->getTableName();
            }
            orm_objects->push_back(tables[i]);
            orm_objects_map[tables[i]->getORMName()]=tables[i];
        }
    }
    for ( int i = 0 ; i < join_fields.size() ; i ++)
    {

        
        if ( join_fields[i].second == JOIN_TYPE::full)
        {
            if ( join_string != "") join_string += " and ";
            if (join_fields[i].first.first.first != "" && join_fields[i].first.first.first !="<<expression>>") join_string += "\""+join_fields[i].first.first.first+"\".";
            if (join_fields[i].first.first.first =="<<expression>>") 
                join_string += join_fields[i].first.first.second+" = ";
            else join_string += "\""+join_fields[i].first.first.second+"\" = ";
            if (join_fields[i].first.second.first != "" && join_fields[i].first.second.first !="<<expression>>") join_string += "\""+join_fields[i].first.second.first+"\".";
            if (join_fields[i].first.second.first =="<<expression>>") 
                join_string += join_fields[i].first.second.second;
            else join_string += "\""+join_fields[i].first.second.second+"\"";
        }
        if ( join_fields[i].second == JOIN_TYPE::cross)
        {
            other_join_string += "cross join lateral ";
            other_join_string += join_fields[i].first.first.first + " as "  + join_fields[i].first.first.second ;
        }
        else  if ( join_fields[i].second == JOIN_TYPE::left_join || join_fields[i].second == JOIN_TYPE::right_join || join_fields[i].second == JOIN_TYPE::inner)
        {
            if ( join_fields[i].second == JOIN_TYPE::left_join) other_join_string += "left join ";
            else if ( join_fields[i].second == JOIN_TYPE::right_join) other_join_string += "right join ";
            else if ( join_fields[i].second == JOIN_TYPE::inner) other_join_string += "inner join ";
            other_join_string += "\""+join_fields[i].first.second.first+"\" on " +
            "\""+join_fields[i].first.second.first+"\"."+
            "\""+join_fields[i].first.second.second+"\" = " +
            "\""+join_fields[i].first.first.first+"\"."+
            "\""+join_fields[i].first.first.second+"\""+
            get_aux_condition(join_fields[i].first.second.first,join_fields);

            other_join_string += "\n";
        }
    }

    from_string = column_names;
    if (other_join_string != "" ) pre_conditions += "\n" + other_join_string;
    if (join_string != "") conditions += " where "+join_string;

}


map <string,PSQLAbstractORM *> * PSQLJoinQueryIterator::next (bool read_only)
{
    if (psqlQuery->fetchNextRow())
    {
        map <string,PSQLAbstractORM *> * results  = new map <string,PSQLAbstractORM *>();
        for (auto orm_object: *orm_objects) 
        {
            PSQLAbstractORM * orm = NULL;
            if ( read_only)
            {
                orm = orm_object->clone();
                orm->set_enforced_partition_number(partition_number);
                orm->assignResults(psqlQuery, read_only);
            }
            else
                orm= psqlController.addToORMCache(orm_object,psqlQuery,partition_number,data_source_name);
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
    // cout << "************> " << orms->size() << endl;

    for (auto orm_pair: (*orms))
        if (orm_pair.second != NULL)
        {
            // cout << orm_pair.first << ": " << orm_pair.second->getORMName() << endl;
            orm_pair.second->unlock_me(true);
        }
    // cout << "************< " << orms->size() << endl;
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

        PSQLJoinQueryPartitionIterator psqlJoinQueryPartitionIterator (psqlQueryPartition,me->orm_objects,me->extras,partition_number);
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
                    // if (aggregate == "")
                    // {
                    //         cout << "Bug is here "<< endl;
                    // }
                    // me->adjust_orms_list(orms_list);
                    // cout << "Partition number: " << partition_number << endl;
                    f(orms_list,partition_number,shared_lock,extras);
                    shared_lock->lock();
                    // cout <<"+++++++orms_list->size(): "<< orms_list->size() << "   "<<  aggregate << endl;
                    for (auto o : *orms_list)
                    {
                        // cout << "Destroying orm row" << endl;
                        me->unlock_orms(o);
                        PSQLGeneric_primitive_orm * gorm = ORM(PSQLGeneric,o);
                        if (gorm != NULL) delete (gorm);
                        delete (o);
                    }
                    shared_lock->unlock();
                    aggregate =  psqlJoinQueryPartitionIterator.exploreNextAggregate();
                    orms_list->clear();
                    // cout << "AFTER Partition number: " << partition_number << endl;
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

        PSQLJoinQueryPartitionIterator psqlJoinQueryPartitionIterator (psqlQueryPartition,me->orm_objects,me->extras,partition_number);
        map <string,PSQLAbstractORM *> *  orms = NULL;
        string aggregate = "";
        bool finished = false;
        int counter = 1;
        // cout << "PROCESS INTERNAL START: " << partition_number << endl;
        do {
            if ( orms!= NULL) delete(orms);

            orms = psqlJoinQueryPartitionIterator.next();
            counter++;

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
        // cout << "PROCESS INTERNAL END: " << partition_number << endl;


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
        for ( int i  = 0 ; i < p->size() ; i ++)
             (*p)[i]->dump();

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
        // cout << "After Threads Creation" << endl;

        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                t->join();
                delete (t);
                delete((*p)[i]);
        }
        time_t time_snapshot2 = time (NULL);
        cout << "Finished multi-threading execution" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
        cout << "cache counter: " << psqlController.getCacheCounter() << endl;
    }
}

void PSQLJoinQueryIterator::process_sequential(std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras){
    time_t start = time (NULL);
    cout << "Executing PSQL Query on the remote server" << endl;
    if (this->execute() && this->psqlQuery->getRowCount() > 0)
    {
        time_t time_snapshot1 = time (NULL);

        cout << "Query results " << this->psqlQuery->getRowCount() << " in "  << (time_snapshot1-start)<< " seconds .."<<endl;
        cout << "Starting execution" << endl;

        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(1);
        mutex shared_lock;
        process_internal(data_source_name,this,(*p)[0],0,&shared_lock,extras,f);
        delete((*p)[0]);

        time_t time_snapshot2 = time (NULL);
        cout << "Finished Process Internal Sequential" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
    }
}

void PSQLJoinQueryIterator::process_aggregate_sequential(std::function<void(vector<map <string,PSQLAbstractORM *> *> * orms,int partition_number,mutex * shared_lock,void * extras)> f,void * extras){
    time_t start = time (NULL);
    cout << "Executing PSQL Query on the remote server" << endl;
    if (this->execute() && this->psqlQuery->getRowCount() > 0)
    {
        time_t time_snapshot1 = time (NULL);

        cout << "Query results " << this->psqlQuery->getRowCount() << " in "  << (time_snapshot1-start)<< " seconds .."<<endl;
        cout << "Starting execution" << endl;

        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(1);

        mutex shared_lock;
        process_internal_aggregate(data_source_name,this,(*p)[0],0,&shared_lock,extras,f);
        delete((*p)[0]);

        time_t time_snapshot2 = time (NULL);
        cout << "Finished Process Internal Aggregate" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
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
        // cout << "After Threads Creation" << endl;

        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                // cout << "BEFORE Joining on thread #" << i << endl;
                t->join();
                // cout << "AFTER Joining on thread #" << i << endl;

                delete (t);
                delete((*p)[i]);
        }
        time_t time_snapshot2 = time (NULL);

        cout << "Finished multi-threading execution" <<  " in "  << (time_snapshot2-time_snapshot1) << " seconds .." << endl;
    }
    time_t time_snapshot1 = time (NULL);
    cout << "Query results " << this->psqlQuery->getRowCount() << " in "  << (time_snapshot1-start)<< " seconds .."<<endl;

    
}
bool PSQLJoinQueryIterator::setDistinct (vector<pair<string,string>> distinct_map)
{
    int count = 0;
    for (auto dm : distinct_map)
    {
        if (orm_objects_map.find(dm.first+"_primitive_orm") != orm_objects_map.end())
        {
            // cout << orm_objects_map[dm.first+"_primitive_orm"]->compose_field_and_alias(dm.second) << endl;
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

bool PSQLJoinQueryIterator::setAggregates (map<string, pair<string, int>> _aggregate_map)
{
    aggregate_map = _aggregate_map;
    int count = 0;
    string aggregate = "";
    aggregate_flag=false;
    for (auto agg : aggregate_map)
    {
        if (orm_objects_map.find(agg.first+"_primitive_orm") != orm_objects_map.end())
        {
            if (agg.second.second == 1) //Integer
            {
                if (count == 0 )
                    aggregate = "concat(";
                else aggregate += ",";
                aggregate+= "lpad("+orm_objects_map[agg.first+"_primitive_orm"]->compose_field(agg.second.first)+"::varchar,10,'0')";
                count ++;
            }else if (agg.second.second == 2) //Date
            {
                if (count == 0 )
                    aggregate = "concat(";
                else aggregate += ",";
                aggregate+= "TO_CHAR("+orm_objects_map[agg.first+"_primitive_orm"]->compose_field(agg.second.first)+", 'YYYYMMDD')";
                count ++;
            }
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



PSQLJoinQueryPartitionIterator::PSQLJoinQueryPartitionIterator (AbstractDBQuery * _psqlQuery,vector <PSQLAbstractORM *> * _orm_objects, map <string,string> _extras,int _partition_number){ 
            psqlQuery = _psqlQuery;
            orm_objects = _orm_objects;
            extras = _extras;
            partition_number = _partition_number;
        }
void PSQLJoinQueryPartitionIterator::reverse()
{
    psqlQuery->fetchPrevRow();
}

string PSQLJoinQueryPartitionIterator::exploreNextAggregate ()
{
    return psqlQuery->getNextValue("aggregate");
}
map <string,PSQLAbstractORM *> * PSQLJoinQueryPartitionIterator::next ()
{
    if (psqlQuery->fetchNextRow())
    {
        map <string,PSQLAbstractORM *> * results  = new map <string,PSQLAbstractORM *>();
        for (auto orm_object: *orm_objects) 
        {
            if ( psqlQuery->getColumnIndex(orm_object->compose_field_with_index(orm_object->getIdentifierName())) >= 0)
            {
                PSQLAbstractORM * orm= psqlController.addToORMCache(orm_object,psqlQuery,partition_number,orm_object->get_data_source_name());
                (*results)[orm->getTableName()] = orm;
            }
            // PSQLAbstractORM * orm = orm_object->clone();
            // orm->set_enforced_partition_number(partition_number);
            // orm->assignResults(psqlQuery);
            // (*results)[orm->getTableName()] = orm;
        }
        if (extras.size() > 0)
        {
            PSQLGeneric_primitive_orm * orm = new PSQLGeneric_primitive_orm("");
            for (auto e : extras)
                orm->add(e.first,psqlQuery->getValue(e.first));
            (*results)["PSQLGeneric"] = orm;            
        }
        else (*results)["PSQLGeneric"] = NULL;
        return results;
    }
    else return NULL;
}
PSQLJoinQueryPartitionIterator::~PSQLJoinQueryPartitionIterator (){}