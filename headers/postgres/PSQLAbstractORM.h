#ifndef PSQLABSTRACTORM_H
#define PSQLABSTRACTORM_H

#include <PSQLConnection.h>

class PSQLAbstractORM 
{
    protected:
        string table_name;
        // PSQLConnection * psqlConnection;
        // AbstractDBQuery * psqlQuery;
        map <string,map<string, string>> relatives_def;
        string identifier_name;
    public:
        virtual string getFromString () = 0;
        virtual void assignResults (AbstractDBQuery * psqlQuery) = 0;
        virtual long getIdentifier() = 0;
        virtual string getIdentifierName();
        PSQLAbstractORM (string _table_name,string _identifier);
        virtual ~PSQLAbstractORM();
};

#endif