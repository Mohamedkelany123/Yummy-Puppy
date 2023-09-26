#ifndef ABSTRACTDBQUERY_H
#define ABSTRACTDBQUERY_H

#include <common.h>

class AbstractDBQuery{
    public:
        AbstractDBQuery(){}
        virtual bool hasResults () = 0;
        virtual bool fetchNextRow() = 0;
        virtual bool fetchPrevRow() = 0;
        virtual int getColumnCount() = 0;
        virtual int getRowCount() = 0;
        virtual string getColumnName(int index) = 0;
        virtual string getColumnType(int index) = 0 ;
        virtual int getColumnSize(int index) = 0;
        virtual string getResultField(int index) = 0;
        virtual string getValue(string column_name) = 0;
        virtual int getColumnIndex(string column_name) = 0;
        virtual map<string, vector<string>> getResultAsString() = 0;
        virtual ~AbstractDBQuery(){}
};

#endif
