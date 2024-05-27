#include <PSQLQuery.h>
#include <PSQLConnection.h>

PSQLQuery::PSQLQuery(PSQLConnection *_psqlConnection, string query)
{
    is_partition= false;
    pgresult = NULL;
    start_index = 0;
    result_count = 0;
    column_count = 0;
    result_index = start_index-1;
    psqlConnection=_psqlConnection;
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
    // else cout << "Error connection went dead" << endl;
}
bool PSQLQuery::hasResults()
{
    return (pgresult != NULL && PQresultStatus(pgresult) == PGRES_TUPLES_OK && result_count > 0);
}

bool PSQLQuery::fetchNextRow()
{
    if (result_index-start_index < result_count - 1)
    {
        result_index++;
        return true;
    }
    return false;
}

bool PSQLQuery::fetchPrevRow()
{
    if (result_index > start_index )
    {
        result_index--;
        return true;
    }
    return false;
}


map<string, vector<string>> PSQLQuery::getResultAsString()
{
    map<string, vector<string>> result_map;

    if (PQresultStatus(pgresult) == 7 || PQresultStatus(pgresult) == 6)
    {

        cout << "Query ERROR: " << endl;
        cout << PQresultErrorMessage(pgresult) << endl
             << "----------------------------\n";
        
        throw std::invalid_argument(PQresultErrorMessage(pgresult));
    }
    try
    {

        if (PQresultStatus(pgresult) == PGRES_TUPLES_OK)
        {
            vector<string> columnNames;

            for (int row = 0; row < result_count; row++)
            {
                for (int col = 0; col < column_count; col++)
                {
                    // cout << "row = " << row << " col = " << col << endl;
                    string col_name = getColumnName(col);
                    columnNames.push_back(col_name);

                    string str_val = getValue(row, col);

                    if (str_val.compare("") == 0)
                    {
                        result_map[col_name].push_back("-"); // any null value become "-"
                    }
                    else
                    {
                        result_map[col_name].push_back(str_val);
                    }
                }
            }
        }

        vector<string> column_order;
        for (int i = 0; i < column_count; i++)
        {
            // cout << "i = " << i << endl;
            column_order.push_back(this->getColumnName(i));
        }
        result_map["column_order"] = column_order;

        return result_map;
    }
    catch (exception &e)
    {
        throw std::invalid_argument("Failed to retrieve data");
    }
}

int PSQLQuery::getColumnCount()
{
    return column_count;
}

int PSQLQuery::getRowCount()
{
    return result_count;
}
int PSQLQuery::getColumnIndex(string column_name)
{
    return PQfnumber(pgresult, column_name.c_str());
}
string PSQLQuery::getColumnName(int index)
{
    if (index < column_count)
        return PQfname(pgresult, index);
    else
        return "";
}

string PSQLQuery::getColumnType(int index)
{
    if (index < column_count)
    {
        string oid = to_string(PQftype(pgresult, index));
        return this->psqlConnection->getTypeFromOID(oid);
    }
    else
        return "";

}
int PSQLQuery::getColumnSize(int index)
{
    if (index < column_count)
        return PQfsize(pgresult, index);
    else
        return 0;

}


string PSQLQuery::getNextResultField(int index)
{

    if (!(result_index-start_index < result_count - 1)) return "";
    result_index ++;
    if (index >=0 && result_index >=0 && index < column_count )
    {
        result_index --;
        return PQgetvalue(pgresult, result_index+1, index);
    }
    else
        return "";
}

string PSQLQuery::getResultField(int index)
{
    if (index >=0 && result_index >=0 && index < column_count )
    {
        return PQgetvalue(pgresult, result_index, index);
    }
    else
        return "";
}
string PSQLQuery::getValue(string column_name)
{
    return getResultField (getColumnIndex(column_name));
}


string PSQLQuery::getNextValue(string column_name)
{
    return getNextResultField (getColumnIndex(column_name));
}



string PSQLQuery::getJSONValue(string column_name)
{
    string s = getResultField (getColumnIndex(column_name));
    if ( s == "") s = "{}";
    s.erase(std::remove(s.begin(), s.end(), '/'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\\'), s.end());
    if ( s[0] == '"')
    {
        s.erase(0, 1);
        s.erase(s.size() - 1);
    }
    return s;
}
string PSQLQuery::getValue(int row_index, int col_index)
{
    return PQgetvalue(pgresult, row_index, col_index);
}
vector <PSQLQueryPartition * > * PSQLQuery::partitionResults (int partition_count)
{
    //cout << "result_count: " << result_count << endl;
    if ( result_count < partition_count ) partition_count = result_count;
    int partition_size = result_count/partition_count;
    int last_partition_size = result_count-(partition_size*(partition_count-1));

    vector <PSQLQueryPartition * > * partitions = new vector <PSQLQueryPartition * >();
    for ( int i = 0 ; i < partition_count ; i ++)
    {
        if ( i != partition_count - 1)
        {
            PSQLQueryPartition * p = new PSQLQueryPartition(pgresult,i*partition_size,partition_size);
            partitions->push_back(p);
        }
        else
        {
            PSQLQueryPartition * p = new PSQLQueryPartition(pgresult,i*partition_size,last_partition_size);
            partitions->push_back(p);
        }
    }
    return partitions;
}

PSQLQuery::~PSQLQuery()
{
    if (pgresult != NULL && !is_partition)
        PQclear(pgresult);
}


PSQLQueryPartition::PSQLQueryPartition (PGresult * _pgresult,int _start_index,int _result_count) : PSQLQuery (NULL,"")
{
    is_partition= true;
    pgresult = _pgresult;
    if (pgresult != NULL )
    {
        start_index = _start_index;
        result_count = _result_count;
        column_count = 0;
        result_index = start_index-1;
        total_result_count = 0 ;
        if (PQresultStatus(pgresult) == PGRES_TUPLES_OK)
        {
            column_count = PQnfields(pgresult);
            total_result_count = PQntuples(pgresult);
        }
    }
}
int PSQLQueryPartition::adjust_for_aggregation (int _start_index)
{
    start_index = _start_index;
    if (start_index == -1)
    {
        result_count  = 0;
        result_index = start_index-1;
        return start_index;
    }
    int end_index = start_index+ result_count-1;
    result_index = end_index;
    // cout << "r_index after: "<< result_index<< endl;
    if (result_index < total_result_count)
    {
        string aggregate = this->getValue("aggregate");
        // cout << "aggregate: " << aggregate<< endl;
        // cout << "r_index before: "<< result_index<< endl;
        do { 
            result_index ++ ;
        } while (result_index < total_result_count && aggregate == this->getValue("aggregate"));
        end_index = result_index-1; 
    }
    else result_index = total_result_count;
    end_index = result_index-1; 
    // cout << "end_index: "<< end_index << endl;
    // cout << "total_result_count: "<< total_result_count << endl;
    // cout << "______________" << endl;
    result_count = end_index - start_index+1;
    result_index = start_index-1;
    if ( end_index + 1 < total_result_count)
        return end_index+1;
    else return -1;
}
void PSQLQueryPartition::dump()
{
    printf ("start: %d\t result_index: %d\t result_count:%d\t end_index: %d\n",start_index,result_index,result_count,start_index+result_count-1);
}

vector <PSQLQueryPartition * > * PSQLQueryPartition::partitionResults (int partition_count)
{
    return NULL;
}
PSQLQueryPartition::~PSQLQueryPartition()
{

}
