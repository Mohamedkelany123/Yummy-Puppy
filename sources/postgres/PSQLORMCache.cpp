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
    cout << "Staring to commit " << endl;
    for (auto orm_cache: cache)
        for (auto orm_cache_item:orm_cache.second) 
            if (orm_cache_item.second->isUpdated())
            {
                cout << "I have one object updated here :)" << endl;
                orm_cache_item.second->update();
            }
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
