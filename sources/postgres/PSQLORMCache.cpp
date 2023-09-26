#include <PSQLORMCache.h>
 

PSQLORMCache::PSQLORMCache()
{

}
void PSQLORMCache::add(string name,PSQLAbstractORM * psqlAbstractORM)
{
    cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
}
void PSQLORMCache::commit()
{

}
void PSQLORMCache::commit(string name)
{

}
void PSQLORMCache::commit(string name,long id)
{

}
void PSQLORMCache::flush()
{

}
void PSQLORMCache::flush(string name)
{

}
void PSQLORMCache::flush(string name,long id)
{

}
PSQLORMCache::~PSQLORMCache()
{
    commit();
    for (auto orm_cache: cache)
        for (auto orm_cache_item:orm_cache.second) 
            delete (orm_cache_item.second);
}
