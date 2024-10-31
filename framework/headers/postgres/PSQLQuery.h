#ifndef PSQLQUERY_H
#define PSQLQUERY_H

#include <AbstractDBQuery.h>

class PSQLConnection;
class PSQLQueryPartition ;

class PSQLQuery : public AbstractDBQuery
{
    protected:
        PGresult *pgresult;
        int result_count;
        int column_count;
        int result_index;
        int start_index;
        bool is_partition;
        PSQLConnection *psqlConnection;
    public:
        PSQLQuery(PSQLConnection * _psqlConnection, string query);
        virtual bool hasResults();
        virtual bool fetchNextRow();
        virtual bool fetchPrevRow();
        virtual int getColumnCount();
        virtual int getRowCount();
        virtual string getValue(string column_name);
        virtual string getNextValue(string column_name);
        virtual string getJSONValue(string column_name);
        virtual string getValue(int col_index, int row_index);
        virtual PGresult *getPGresult() { return pgresult; }
        virtual int getColumnIndex(string column_name);
        virtual string getColumnName(int index);
        virtual string getColumnType(int index);
        virtual int getColumnSize(int index);
        virtual string getResultField(int index);
        virtual string getNextResultField(int index);
        virtual map<string, vector<string>> getResultAsString();
        virtual vector <PSQLQueryPartition * > * partitionResults (int partition_count);
        virtual ~PSQLQuery();
};


class PSQLQueryPartition : public PSQLQuery
{
    private:
        int total_result_count;
    public:
        PSQLQueryPartition (PGresult * _pgresult,int _start_index,int result_count);
        virtual vector <PSQLQueryPartition * > * partitionResults (int partition_count);
        int adjust_for_aggregation (int _start_index);
        void dump();
        ~PSQLQueryPartition();
};


#endif