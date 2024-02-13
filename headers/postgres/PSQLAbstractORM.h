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
        map <string,pair<string,bool>> insert_default_values;
        map <string,pair<string,bool>> update_default_values;
        string data_source_name;
        bool orm_transactional;


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
        virtual void addDefault(string name,string value, bool is_insert = true, bool is_func=false);
        PSQLAbstractORM (string _data_source_name, string _table_name,string _identifier, bool exclude_from_transactional);
        virtual PSQLAbstractORM * clone ()=0;
        virtual void lock_me();
        virtual void unlock_me(bool restrict_to_owner = false);        
        bool isOrmTransactional();
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