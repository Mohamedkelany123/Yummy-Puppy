#ifndef PSQLORMCACHE_H
#define PSQLORMCACHE_H
#include <PSQLAbstractORM.h>

class PSQLORMCache
{
    private:
        map <string,map <long,PSQLAbstractORM *>> update_cache;
        map <string,vector <PSQLAbstractORM *>> insert_cache;

        vector <map <PSQLAbstractORM *,PSQLAbstractORM *>> update_thread_cache;
        vector <map <PSQLAbstractORM *,PSQLAbstractORM *>> insert_thread_cache;
        int threads_count;
        int insert_cache_items_count;
        int update_cache_items_count;
        
        std::mutex lock;
        static bool commit_parallel_internal (PSQLORMCache * me,int t_index,mutex * shared_lock,PSQLConnection * _psqlConnection,vector <bool> * threads_results, bool transactional, bool orm_transaction);
    public:
        PSQLORMCache();
        void set_threads_count (int _threads_count);
        int search_update_thread_cache(PSQLAbstractORM * psqlAbstractORM);
        bool doesExist (string name,long _identifier);
        PSQLAbstractORM * fetch (string name,long _identifier);
        PSQLAbstractORM * add(PSQLAbstractORM * seeder,AbstractDBQuery * psqlQuery,int partition_number=-1);
        PSQLAbstractORM * add(string name,PSQLAbstractORM * psqlAbstractORM);
        bool release(string name,PSQLAbstractORM * psqlAbstractORM);
        void release();
        void commit_parallel (string data_source_name, bool transaction=false, bool orm_transaction = true);
        void commit_sequential (string data_source_name, bool transaction=false, bool clean_updates=false);
        void commit( string data_source_name, bool parallel=false,bool transaction=false, bool clean_updates=false);
        void commit(string data_source_name, string name);
        void commit(string data_source_name, string name,long id);
        void flush();
        void flush(string name);
        void flush(string name,long id);
        void unlock_current_thread_orms();
        void clear_cache(bool clean_updates=false);

        ~PSQLORMCache();
};




#endif