#include <PSQLQuery.h>
#include <PSQLConnection.h>

PSQLQuery::PSQLQuery(PSQLConnection *_psqlConnection, string query)
{
    pgresult = NULL;
    result_count = 0;
    column_count = 0;
    result_index = -1;
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
}
bool PSQLQuery::hasResults()
{
    return (pgresult != NULL && PQresultStatus(pgresult) == PGRES_TUPLES_OK && result_count > 0);
}

bool PSQLQuery::fetchNextRow()
{
    if (result_index < result_count - 1)
    {
        result_index++;
        return true;
    }
    return false;
}

bool PSQLQuery::fetchPrevRow()
{
    if (result_index > 0 )
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
string PSQLQuery::getValue(int row_index, int col_index)
{
    return PQgetvalue(pgresult, row_index, col_index);
}

PSQLQuery::~PSQLQuery()
{
    if (pgresult != NULL)
        PQclear(pgresult);
}
