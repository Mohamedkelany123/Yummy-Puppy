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
}
void PSQLAbstractQueryIterator::setNativeSQL(string _sql)
{
    sql=_sql;
}
void PSQLAbstractQueryIterator::filter (const Expression & e)
{
    if ( conditions == "")
        conditions = " where " + e.generate();
    else conditions += " and "+ e.generate();
}
bool PSQLAbstractQueryIterator::execute()
{
    if (psqlConnection == NULL ) return false;
    if (orderby_string == "")
        sql = "select "+from_string+" from "+ table_name + conditions ;//+" order by loan_app_loan.id";
    else sql = "select "+from_string+" from "+ table_name + conditions +" order by "+orderby_string;
    // cout << sql << endl;
    psqlQuery = psqlConnection->executeQuery(sql);
    if (psqlQuery != NULL) return true;
    else return false;
}
PSQLAbstractQueryIterator::~PSQLAbstractQueryIterator()
{
    psqlController.releaseConnection(data_source_name,psqlConnection);
    if ( psqlQuery != NULL) delete (psqlQuery);
}


PSQLJoinQueryIterator::PSQLJoinQueryIterator(string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields):
PSQLAbstractQueryIterator(_data_source_name,"")
{
    orm_objects = new vector <PSQLAbstractORM *> ();
    for ( int i =0 ; i  < tables.size() ; i ++)
    {
        if ( column_names != "") column_names += ",";
        column_names += tables[i]->getFromString();
        if ( table_name != "") table_name += ",";
        table_name += tables[i]->getTableName();
        orm_objects->push_back(tables[i]);
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
map <string,PSQLAbstractORM *> * PSQLJoinQueryIterator::next ()
{
    if (psqlQuery->fetchNextRow())
    {
        map <string,PSQLAbstractORM *> * results  = new map <string,PSQLAbstractORM *>();
        for (auto orm_object: *orm_objects) 
        {
            PSQLAbstractORM * orm = orm_object->clone();
            orm->assignResults(psqlQuery);
            (*results)[orm->getTableName()] = orm;
        }
        return results;
    }
    else return NULL;
}

void PSQLJoinQueryIterator::unlock_orms (map <string,PSQLAbstractORM *> *  orms)
{
    for (auto orm_pair: (*orms))
        orm_pair.second->unlock_me();

}

void  PSQLJoinQueryIterator::process_internal(PSQLJoinQueryIterator * me,PSQLQueryPartition * psqlQueryPartition,int partition_number,mutex * shared_lock,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock)> f)
{
        PSQLJoinQueryPartitionIterator psqlJoinQueryPartitionIterator (psqlQueryPartition,me->orm_objects);
        map <string,PSQLAbstractORM *> *  orms = NULL;
        do {
            if ( orms!= NULL) delete(orms);
            orms =psqlJoinQueryPartitionIterator.next();
            if (orms != NULL) 
            {
               f(orms,partition_number,shared_lock);
               me->unlock_orms(orms);
            }
        } while (orms != NULL);

}

void PSQLJoinQueryIterator::process(int partitions_count,std::function<void(map <string,PSQLAbstractORM *> * orms,int partition_number,mutex * shared_lock)> f)
{
    if (this->execute() && this->psqlQuery->getRowCount() > 0)
    {
        vector <PSQLQueryPartition * > * p = ((PSQLQuery *)this->psqlQuery)->partitionResults(partitions_count);
        vector <thread *> threads;
        mutex shared_lock;
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
            thread * t = new thread(process_internal,this,(*p)[i],i,&shared_lock,f);
            threads.push_back(t);
        }
        for ( int i  = 0 ; i < p->size() ; i ++)
        {
                thread * t = threads[i];
                t->join();
                delete (t);
                delete((*p)[i]);
        }
    }
}

PSQLJoinQueryIterator::~PSQLJoinQueryIterator()
{
    for (auto orm_object: *orm_objects) 
        delete (orm_object);
    delete (orm_objects);

}