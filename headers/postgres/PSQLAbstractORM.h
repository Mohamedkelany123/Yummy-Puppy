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
        mutex lock;
        string locking_thread;
    public:
        virtual string getFromString () = 0;
        virtual void assignResults (AbstractDBQuery * psqlQuery,bool _read_only = false) = 0;
        virtual long getIdentifier() = 0;
        virtual bool isUpdated() = 0;
        virtual bool update(PSQLConnection * _psqlConnection=NULL) = 0;
        virtual long insert(PSQLConnection * _psqlConnection=NULL) = 0;
        virtual string getIdentifierName();
        virtual string getTableName();
        virtual bool isLoaded();
        PSQLAbstractORM (string _table_name,string _identifier);
        virtual PSQLAbstractORM * clone ()=0;
        virtual void lock_me();
        virtual void unlock_me(bool restrict_to_owner = false);        
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