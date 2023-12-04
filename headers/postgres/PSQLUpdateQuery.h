#ifndef PSQLUPDATEQUERY_H
#define PSQLUPDATEQUERY_H

#include <PSQLConnection.h>
#include <PSQLController.h>
#include <PSQLAbstractQueryIterator.h>


class PSQLUpdateQuery 
{
    protected:
        PGresult *pgresult;
        string conditions;
        string set_string;
        string table_name;
        string data_source_name;
    public:
        PSQLUpdateQuery(string _data_source_name,string _table_name,Expression const & e,map <string,string> set_map);
        void filter ( Expression const & e);
        void set (map <string,string> set_map);
        bool update (PSQLConnection * _psqlConnection = NULL);
        virtual ~PSQLUpdateQuery();
};

#endif