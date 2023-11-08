#include <PSQLORMCache.h>
 

PSQLORMCache::PSQLORMCache()
{

}
PSQLAbstractORM * PSQLORMCache::add(string name,PSQLAbstractORM * psqlAbstractORM)
{
    std::lock_guard<std::mutex> guard(lock);
    PSQLAbstractORM * orm = NULL;

    if (psqlAbstractORM->getIdentifier() == -1 )
        insert_cache[name].push_back(psqlAbstractORM);
    else
    { 
        if (update_cache.find(name) != update_cache.end()) 
            if (update_cache[name].find(psqlAbstractORM->getIdentifier()) != update_cache[name].end()) {
                orm = update_cache[name][psqlAbstractORM->getIdentifier()];
                orm->lock_me();
                // cout << "replacing an ORM of type "<< name << endl;
            }
        update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
    }
    return orm;
}
bool PSQLORMCache::release(string name,PSQLAbstractORM * psqlAbstractORM)
{
    std::lock_guard<std::mutex> guard(lock);
    PSQLAbstractORM * orm = NULL;
    if (psqlAbstractORM->getIdentifier() == -1 ) return false;
    else  if (update_cache.find(name) != update_cache.end()) 
    {
            if (update_cache[name].find(psqlAbstractORM->getIdentifier()) != update_cache[name].end()) {
                orm = update_cache[name][psqlAbstractORM->getIdentifier()];
                orm->unlock_me();
                return true;
            }
    }
    return false;
}
void PSQLORMCache::release()
{
    std::lock_guard<std::mutex> guard(lock);
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            orm_cache_item.second->unlock_me();
    for (auto orm_cache: insert_cache)
        for (auto orm_cache_item:orm_cache.second) 
                orm_cache_item->unlock_me();
}
void PSQLORMCache::commit()
{
    cout << "Staring to commit " << endl;
    long counter = 0;
    std::lock_guard<std::mutex> guard(lock);
    for (auto orm_cache: insert_cache)
    {
        for (auto orm_cache_item:orm_cache.second) 
        {
                orm_cache_item->lock_me();
                orm_cache_item->insert();
                orm_cache_item->unlock_me();
                delete (orm_cache_item);
                counter ++;
                if (counter % 1000 == 0 )
                    cout << "Committed " << counter << "inserts" << endl;
        }
        insert_cache[orm_cache.first].clear();
    }
    counter = 0;
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            if (orm_cache_item.second->isUpdated())
            {
                orm_cache_item.second->lock_me();
                orm_cache_item.second->update();
                orm_cache_item.second->unlock_me();
                counter ++;
                if (counter % 1000 == 0 )
                    cout << "Committed " << counter << "updates" << endl;
            }
    cout << "Exiting commit" << endl;

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
    std::lock_guard<std::mutex> guard(lock);
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            delete (orm_cache_item.second);
    for (auto orm_cache: insert_cache)
        for (auto orm_cache_item:orm_cache.second) 
                delete (orm_cache_item);
}
