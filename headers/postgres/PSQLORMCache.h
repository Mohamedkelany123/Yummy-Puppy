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
        static bool commit_parallel_internal (PSQLORMCache * me,int t_index,mutex * shared_lock,PSQLConnection * _psqlConnection,vector <bool> * threads_results);
    public:
        PSQLORMCache();
        void set_threads_count (int _threads_count);
        int search_update_thread_cache(PSQLAbstractORM * psqlAbstractORM);
        PSQLAbstractORM * add(string name,PSQLAbstractORM * psqlAbstractORM);
        bool release(string name,PSQLAbstractORM * psqlAbstractORM);
        void release();
        void commit_parallel (bool transaction=false);
        void commit_sequential (bool transaction=false);
        void commit(bool parallel=false,bool transaction=false);
        void commit(string name);
        void commit(string name,long id);
        void flush();
        void flush(string name);
        void flush(string name,long id);
        void unlock_current_thread_orms();
        ~PSQLORMCache();
};




#endif