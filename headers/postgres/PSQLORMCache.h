#ifndef PSQLORMCACHE_H
#define PSQLORMCACHE_H
#include <PSQLAbstractORM.h>

class PSQLORMCache
{
    private:
        map <string,map <long,PSQLAbstractORM *>> update_cache;
        map <string,vector <PSQLAbstractORM *>> insert_cache;
        std::mutex lock;
    public:
        PSQLORMCache();
        void add(string name,PSQLAbstractORM * psqlAbstractORM);
        void commit();
        void commit(string name);
        void commit(string name,long id);
        void flush();
        void flush(string name);
        void flush(string name,long id);
        ~PSQLORMCache();
};




#endif