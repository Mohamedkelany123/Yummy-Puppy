#ifndef PSQLABSTRACTORM_H
#define PSQLABSTRACTORM_H

#include <PSQLConnection.h>

class PSQLAbstractORM 
{
    protected:
        string table_name;
        PSQLConnection * psqlConnection;
        AbstractDBQuery * psqlQuery;
        map <string,map<string, string>> relatives_def;
    public:
        virtual string getFromString () = 0;
        virtual void assignResults () = 0;
        virtual long getIdentifier() = 0;
        PSQLAbstractORM (string table_name);
        virtual ~PSQLAbstractORM();
};

#endif