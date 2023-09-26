#ifndef PSQLQUERY_H
#define PSQLQUERY_H

#include <AbstractDBQuery.h>

class PSQLConnection;

class PSQLQuery : public AbstractDBQuery
{
    private:
        PGresult *pgresult;
        int result_count;
        int column_count;
        int result_index;
        PSQLConnection *psqlConnection;
    public:
        PSQLQuery(PSQLConnection * _psqlConnection, string query);
        bool hasResults();
        bool fetchNextRow();
        bool fetchPrevRow();
        int getColumnCount();
        int getRowCount();
        string getValue(string column_name);
        string getValue(int col_index, int row_index);
        PGresult *getPGresult() { return pgresult; }
        int getColumnIndex(string column_name);
        string getColumnName(int index);
        string getColumnType(int index);
        int getColumnSize(int index);
        string getResultField(int index);
        map<string, vector<string>> getResultAsString();
        ~PSQLQuery();
};

#endif