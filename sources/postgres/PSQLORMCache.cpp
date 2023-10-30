#include <PSQLORMCache.h>
 

PSQLORMCache::PSQLORMCache()
{

}
void PSQLORMCache::add(string name,PSQLAbstractORM * psqlAbstractORM)
{
    lock.lock();
    if (psqlAbstractORM->getIdentifier() == -1 )
        insert_cache[name].push_back(psqlAbstractORM);
    else update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
    lock.unlock();
}
void PSQLORMCache::commit()
{
    cout << "Staring to commit " << endl;
    lock.lock();
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            if (orm_cache_item.second->isUpdated())
                orm_cache_item.second->update();
    lock.unlock();

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
    lock.lock();
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            delete (orm_cache_item.second);
    lock.unlock();
}
