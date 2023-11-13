#include <PSQLORMCache.h>
#include <PSQLController.h>
 

bool PSQLORMCache::commit_parallel_internal (PSQLORMCache * me,int t_index,mutex * shared_lock,PSQLConnection * _psqlConnection)
{
    int counter =0;
    for (auto orm_cache_item: me->insert_thread_cache[t_index])
    {
        orm_cache_item.second->lock_me();
        orm_cache_item.second->insert(_psqlConnection);
        orm_cache_item.second->unlock_me();
        counter ++;
        if (counter % 1000 == 0 )
        {
            shared_lock->lock();
            cout << "Thread # " << t_index <<" Committed " << counter << " inserts" << endl;
            shared_lock->unlock();
        }
    }

    for (auto orm_cache_item: me->update_thread_cache[t_index])
    {
        orm_cache_item.second->lock_me();
        orm_cache_item.second->update(_psqlConnection);
        orm_cache_item.second->unlock_me();
        counter ++;
        if (counter % 1000 == 0 )
            cout << "Thread # " << t_index <<" Committed " << counter << " updates" << endl;
    }
    return true;
}


PSQLORMCache::PSQLORMCache()
{
        insert_cache_items_count=0;
        update_cache_items_count=0;
}
void PSQLORMCache::set_threads_count (int _threads_count)
{
    threads_count = _threads_count;
}

int PSQLORMCache::search_update_thread_cache(PSQLAbstractORM * psqlAbstractORM)
{
    for ( int i = 0 ; i < threads_count ; i ++)
    {
        if (update_thread_cache[i].find(psqlAbstractORM) != update_thread_cache[i].end()) 
            return i;
    }
    return -1;
}

PSQLAbstractORM * PSQLORMCache::add(string name,PSQLAbstractORM * psqlAbstractORM)
{
    std::lock_guard<std::mutex> guard(lock);
    PSQLAbstractORM * orm = NULL;

    if (psqlAbstractORM->getIdentifier() == -1 )
    {
        insert_cache[name].push_back(psqlAbstractORM);
        insert_thread_cache[insert_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
        insert_cache_items_count++;
    }
    else
    { 
        if (update_cache.find(name) != update_cache.end()) 
        {
            if (update_cache[name].find(psqlAbstractORM->getIdentifier()) != update_cache[name].end()) {
                orm = update_cache[name][psqlAbstractORM->getIdentifier()];
                update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
                int t_index = search_update_thread_cache(orm);
                if ( t_index != -1)
                {
                    update_thread_cache[t_index].erase(orm);
                    update_thread_cache[t_index][psqlAbstractORM]=psqlAbstractORM;
                }
                else
                {  
                    update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
                    update_cache_items_count++;
                }

                lock.unlock();
                psqlAbstractORM->lock_me();
                orm->lock_me();
                lock.lock();
                // cout << "replacing an ORM of type "<< name << endl;
            }
            else
            {
                psqlAbstractORM->lock_me();
                // std::ostringstream ss;
                // ss << std::this_thread::get_id() ;
                update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
                update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
                update_cache_items_count++;
            }
        }
        else
        {
            psqlAbstractORM->lock_me();
            // std::ostringstream ss;
            // ss << std::this_thread::get_id() ;
            // printf("assigning new %p for old %p   -   %s\n",psqlAbstractORM,orm,ss.str().c_str());
            update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
            update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
            update_cache_items_count++;
        }
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


void PSQLORMCache::commit_parallel (PSQLConnection * _psqlConnection)
{
    vector <thread *> threads;
    mutex shared_lock;
    for ( int i  = 0 ; i < threads_count ; i ++)
    {
        thread * t = new thread(commit_parallel_internal,this,i,&shared_lock,_psqlConnection);
        threads.push_back(t);
    }
    for ( int i  = 0 ; i < threads_count ; i ++)
    {
            thread * t = threads[i];
            t->join();
            delete (t);
    }
    for ( int i = 0 ; i < threads_count ; i ++)
        insert_thread_cache[i].clear();

}
void PSQLORMCache::commit_sequential (PSQLConnection * _psqlConnection)
{
    long counter = 0;
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
                    cout << "Committed " << counter << " inserts" << endl;
        }
        insert_cache[orm_cache.first].clear();
    }
    for ( int i = 0 ; i < threads_count ; i ++)
        insert_thread_cache[i].clear();

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
                    cout << "Committed " << counter << " updates" << endl;
            }
}



void PSQLORMCache::commit(bool parallel,bool transaction)
{
    cout << "Staring to commit " << endl;
    std::lock_guard<std::mutex> guard(lock);
    PSQLConnection * psqlConnection = NULL;
    if (transaction)
    {
        psqlConnection = psqlController.getPSQLConnection("main");
        psqlConnection->startTransaction();
    }
    if ( parallel ) commit_parallel (psqlConnection);
    else commit_sequential(psqlConnection);
    if (transaction)
    {
        // psqlConnection->commitTransaction();
        psqlController.releaseConnection("main",psqlConnection);
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
