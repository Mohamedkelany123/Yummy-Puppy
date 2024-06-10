#ifndef PSQLABSTRACTORM_H
#define PSQLABSTRACTORM_H

#include <PSQLConnection.h>

class PSQLAbstractORM 
{
    protected:
        string table_name;
        string orm_name;
        int table_index;
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
        map <string, PSQLAbstractORM *> add_references;
        map <string, PSQLAbstractORM *> update_references;
        map <string, int > reference_values;
        bool inserted;
        map <string,string> extras;
        bool cached;
        int enforced_partition_number;

    public:
        virtual string getFromString () = 0;
        virtual void assignResults (AbstractDBQuery * psqlQuery,bool _read_only = false) = 0;
        virtual long getIdentifier() = 0;
        virtual void setIdentifier(long id) = 0;
        virtual bool isUpdated() = 0;
        virtual bool update(PSQLConnection * _psqlConnection=NULL) = 0;
        virtual long insert(PSQLConnection * _psqlConnection=NULL) = 0;
        virtual string getIdentifierName();
        virtual long getIdentifier (AbstractDBQuery * _psqlQuery);
        virtual string getTableName();
        virtual string getORMName();
        virtual bool isLoaded();
        string get_data_source_name ();
        virtual void addDefault(string name,string value, bool is_insert = true, bool is_func=false);
        PSQLAbstractORM (string _data_source_name, string _table_name,string _identifier, bool orm_transactional,int _enforced_partition_number=-1);
        PSQLAbstractORM (const PSQLAbstractORM & _psqlAbstractORM);
        virtual PSQLAbstractORM * clone ()=0;
		virtual string serialize (PSQLConnection * _psqlConnection=NULL)=0;
		virtual void deSerialize (json orm_json,bool _read_only = false)=0;
	    virtual void resolveReferences ()=0;
        virtual void lock_me(bool skip_owner = false);
        virtual void unlock_me(bool restrict_to_owner = false);        
        bool isOrmTransactional();
        void setAddRefernce (string field_name,PSQLAbstractORM * reference);
        void setUpdateRefernce (string field_name,PSQLAbstractORM * reference);
        void commitAddReferences (PSQLConnection * _psqlConnection=NULL);
        void commitUpdateReferences (PSQLConnection * _psqlConnection=NULL);
        string compose_field_and_alias (string field_name);
        string compose_field (string field_name);
        string compose_field_with_index (string field_name);
        void setExtra (string fname, string fvalue);
        string getExtra (string fname);
        virtual void operator = (const PSQLAbstractORM & _psqlAbstractORM);
        virtual void operator = (const PSQLAbstractORM * _psqlAbstractORM);
        int get_enforced_partition_number();
        void set_enforced_partition_number(int _enforced_partition_number);
        
        virtual ~PSQLAbstractORM();
};

class PSQLGeneric_primitive_orm: public PSQLAbstractORM
{
    private:
        map <string, string> values;
    public:
        PSQLGeneric_primitive_orm (string _data_source_name):PSQLAbstractORM (_data_source_name, "","", false){}
        void add (string name, string value){
            values[name] = value;
        }
        string get (string name){
            return values[name];
        }
        map <string, string> getExtraFieldsMap(){return values;}
        int toInt (string name) { return atoi(values[name].c_str());}
        float toFloat (string name) { return atof(values[name].c_str());}
        double toLong (string name) { return atol(values[name].c_str());}
        bool toBool (string name) { return (values[name] == "t");}
        json toJson (string name) { return (nlohmann::json::parse(values[name]));}
        // double toDouble (name) { return atoll(values[name].c_str());} need to be implemented using strtod
        string getFromString () { return "";}
        void assignResults (AbstractDBQuery * psqlQuery,bool _read_only = false) {}
        bool update(PSQLConnection * _psqlConnection=NULL) { return false;}
        long insert(PSQLConnection * _psqlConnection=NULL) { return -1;}
		string serialize (PSQLConnection * _psqlConnection=NULL) { return "";}
		void deSerialize (json orm_json,bool _read_only = false){}
	    void resolveReferences () {}
        long getIdentifier() {return -1;};
        void setIdentifier(long id) {};
        bool isUpdated() {return false;};
        PSQLAbstractORM * clone () { return NULL;}
        ~PSQLGeneric_primitive_orm(){}
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