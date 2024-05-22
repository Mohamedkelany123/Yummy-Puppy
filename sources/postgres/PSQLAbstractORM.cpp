#include <PSQLAbstractORM.h>
#include <PSQLController.h>

PSQLAbstractORM::PSQLAbstractORM (const PSQLAbstractORM & _psqlAbstractORM)
{
       (*this) = _psqlAbstractORM;
}

void PSQLAbstractORM::operator = (const PSQLAbstractORM & _psqlAbstractORM)
{
        table_name = _psqlAbstractORM.table_name;
        orm_name =  _psqlAbstractORM.orm_name;
        table_index = _psqlAbstractORM.table_index;
        relatives_def = _psqlAbstractORM.relatives_def;
        identifier_name = _psqlAbstractORM.identifier_name;
        loaded = false ;
        locking_thread = "";
        insert_default_values = _psqlAbstractORM.insert_default_values;
        update_default_values = _psqlAbstractORM.update_default_values;
        data_source_name = _psqlAbstractORM.data_source_name;
        orm_transactional =  _psqlAbstractORM.orm_transactional;
        add_references = _psqlAbstractORM.add_references;
        update_references = _psqlAbstractORM.update_references;
        inserted = false;
}


PSQLAbstractORM::PSQLAbstractORM (string _data_source_name, string _table_name,string _identifier_name, bool _orm_transactional)
{   
    orm_transactional = _orm_transactional;
    table_name = _table_name;
    orm_name = table_name +"_primitive_orm";
    identifier_name = _identifier_name;
    loaded = false;
    locking_thread = "";
    inserted = false; 
    // psqlConnection = new PSQLConnection ("localhost",5432,"django_ostaz_15082023_old","postgres","postgres");
    // psqlQuery = NULL;
    // map<string, vector<string>> results  = psqlQuery->getResultAsString();
    // for (auto result: results) {
    //     std::cout << "ttt" << std::endl;
    //     std::cout << result.first  << std::endl;
    //     for (auto value: result.second) 
    //         std::cout << "\t\t" << value << std::endl;
    // }
    insert_default_values = psqlController.getInsertDefaultValues();
    update_default_values = psqlController.getUpdateDefaultValues();
    data_source_name = _data_source_name;
}

bool PSQLAbstractORM::isOrmTransactional(){
    return orm_transactional;
}

string PSQLAbstractORM::getIdentifierName ()
{
    return identifier_name;
}

string PSQLAbstractORM::getTableName()
{
    return table_name;
}
string PSQLAbstractORM::getORMName()
{
    return orm_name;
}

bool PSQLAbstractORM::isLoaded ()
{
    return loaded;
}

void PSQLAbstractORM::addDefault(string name,string value, bool is_insert, bool is_func)
{
    pair<string, bool> p; 
    p.first = value;
    p.second = is_func;
    if ( is_insert )
        insert_default_values[name]= p;
    else update_default_values[name]= p;
}

void PSQLAbstractORM::lock_me()
{
    std::ostringstream ss;
    ss << std::this_thread::get_id() ;
//    printf ("lock_me: %p  -   %s \n",this,ss.str().c_str()); 
    lock.lock();
    locking_thread = ss.str();
}
void PSQLAbstractORM::unlock_me(bool restrict_to_owner)
{
    if ( locking_thread == "") return ;
    std::ostringstream ss;
    ss << std::this_thread::get_id() ;

//    printf ("unlock_me: %p  -   %s \n",this,ss.str().c_str()); 
    if (restrict_to_owner)
        if (ss.str() != locking_thread) return;
    locking_thread = "";
    lock.try_lock();
    lock.unlock();
}

void PSQLAbstractORM::setAddRefernce (string field_name,PSQLAbstractORM * reference)
{
    add_references[field_name] = reference;
}
void PSQLAbstractORM::setUpdateRefernce (string field_name,PSQLAbstractORM * reference)
{
    update_references[field_name] = reference;
}


void PSQLAbstractORM::commitAddReferences ()
{
    for (auto& ref : this->add_references) 
    {
        int reference_orm_id = ref.second->insert();
        reference_values[ref.first] = reference_orm_id;
    }
}

void PSQLAbstractORM::commitUpdateReferences ()
{
    for (auto& ref : this->update_references) 
    {
        int reference_orm_id = ref.second->insert();
        reference_values[ref.first] = reference_orm_id;
    }
}

string PSQLAbstractORM::compose_field_and_alias (string field_name)
{

    string str = "`"; 
    str += table_name;
    str += "`.`";
    str += field_name;
    str += "` as \"";
    str += to_string(table_index);
    str += "_";
    str += field_name;
    str += "\"";
    return str;

}

void PSQLAbstractORM::setExtra (string fname, string fvalue)
{
    extras [fname] = fvalue;

}
string PSQLAbstractORM::getExtra (string fname)
{
    if (extras.find(fname) != extras.end())
        return  extras [fname];
    else return "";
}
PSQLAbstractORM::~PSQLAbstractORM()
{
    // cout << "PSQLAbstractORM::~PSQLAbstractORM()" << endl;
    // if (psqlConnection != NULL) delete (psqlConnection);
    // if (psqlQuery != NULL) delete (psqlQuery);
    unlock_me();
    // std::ostringstream ss;
    // ss << std::this_thread::get_id() ;
    // printf ("deleting: %p  -   %s \n",this,ss.str().c_str()); 
}


PSQLQueryJoin::PSQLQueryJoin (string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields):
    PSQLQuery(NULL, "")
{
    string column_names = "";
    string table_names = "";
    string join_string = "";
    for ( int i =0 ; i  < tables.size() ; i ++)
    {
        if ( column_names != "") column_names += ",";
        column_names += tables[i]->getFromString();
        if ( table_names != "") table_names += ",";
        table_names += tables[i]->getTableName();
    }
    for ( int i = 0 ; i < join_fields.size() ; i ++)
    {
        cout << join_fields[i].first.first << endl;
        cout << join_fields[i].first.second << endl;
        cout << join_fields[i].second.first << endl;
        cout << join_fields[i].second.second << endl;
        if ( join_string != "") join_string += " and ";
        join_string += "\""+join_fields[i].first.first+"\"."+
        "\""+join_fields[i].first.second+"\" = "+
        "\""+join_fields[i].second.first+"\"."+
        "\""+join_fields[i].second.second+"\"";
    }

    string query = "select "+ column_names + " from " + table_names + " where " + join_string; 

    psqlConnection = psqlController.getPSQLConnection(_data_source_name);
    if (psqlConnection != NULL && psqlConnection->isAlive())
    {
        pgresult = PQexec(psqlConnection->getPGConnection(), query.c_str());

        if (PQresultStatus(pgresult) == 7 || PQresultStatus(pgresult) == 6)
        {

            cout << "Query ERROR: " << query << endl;
            cout << PQresultErrorMessage(pgresult) << endl
                 << "----------------------------\n";
        }

        if (PQresultStatus(pgresult) == PGRES_TUPLES_OK)
        {
            column_count = PQnfields(pgresult);
            result_count = PQntuples(pgresult);
        }
    }


}


vector <PSQLAbstractORM *> * PSQLQueryJoin::getORMs()
{
    return orms;

}
PSQLQueryJoin::~PSQLQueryJoin ()
{

}
