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
        bool loaded ;
    public:
        virtual string getFromString () = 0;
        virtual void assignResults (AbstractDBQuery * psqlQuery) = 0;
        virtual long getIdentifier() = 0;
        virtual bool isUpdated() = 0;
        virtual bool update() = 0;
        virtual string getIdentifierName();
        virtual string getTableName();
        virtual bool isLoaded();
        PSQLAbstractORM (string _table_name,string _identifier);
        virtual PSQLAbstractORM * clone ()=0;
        virtual ~PSQLAbstractORM();
};



class PSQLQueryJoin : public PSQLQuery
{
    private:
        vector <PSQLAbstractORM *> * orms;
    public:
        PSQLQueryJoin (string _data_source_name,vector <PSQLAbstractORM *> const & tables,vector <pair<pair<string,string>,pair<string,string>>> const & join_fields);
        vector <PSQLAbstractORM *> * getORMs();
        // (PSQLConnection * _psqlConnection, string query);
        ~PSQLQueryJoin ();

};

#endif