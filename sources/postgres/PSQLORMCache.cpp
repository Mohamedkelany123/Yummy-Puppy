#include <PSQLORMCache.h>
#include <PSQLController.h>
 

bool PSQLORMCache::commit_parallel_internal (PSQLORMCache * me,int t_index,mutex * shared_lock,PSQLConnection * _psqlConnection,vector <bool> * threads_results, bool transactional, bool orm_transaction)
{
    int counter =0;
    int return_flag = true;

    // cout << "Started commit internal for " << t_index 
    //     << " || inserts = " << (t_index < me->insert_thread_cache.size() ? me->insert_thread_cache[t_index].size() : 0) 
    //     << " || updates = " <<  (t_index < me->update_thread_cache.size() ? me->update_thread_cache[t_index].size() : 0) << endl;

    if (me->insert_thread_cache.size() > t_index)
    {
        for (auto orm_cache_item: me->insert_thread_cache[t_index])
        {
//            cout << transactional << "& (" << orm_transaction << "==" << orm_cache_item.second->isOrmTransactional() << ") || " << !transactional << ")" << endl;
            if((transactional && (orm_transaction == orm_cache_item.second->isOrmTransactional())) || !transactional){
                orm_cache_item.second->lock_me();
                if (orm_cache_item.second->insert(_psqlConnection) == -1)
                {
                    cout << "Insert problem " << endl;
                    return_flag = false;
                }
                orm_cache_item.second->unlock_me();
                counter ++;
                if (counter % 1000 == 0 )
                {
                    shared_lock->lock();
                    // if(orm_transaction)
                    //     cout << "Transaction Thread # " << t_index <<" Committed " << counter << " inserts" << endl;
                    // else cout << "Non-Transaction Thread # " << t_index <<" Committed " << counter << " inserts" << endl;
                    shared_lock->unlock();
                }
            }
        }
    }


    if (me->update_thread_cache.size() > t_index)
    {
        for (auto orm_cache_item: me->update_thread_cache[t_index])
        {
            if((transactional && (orm_transaction == orm_cache_item.second->isOrmTransactional())) || !transactional){

                if (orm_cache_item.second->isUpdated())
                {
                    orm_cache_item.second->lock_me();
                    if (!orm_cache_item.second->update(_psqlConnection)) return_flag = false;
                    orm_cache_item.second->unlock_me();
                }
                counter ++;
                if (counter % 10000 == 0 )
                {
                    shared_lock->lock();
                    if(orm_transaction)
                        cout << "Transaction Thread # " << t_index <<" Committed " << counter << " updates" << endl;
                    else cout << "Non-Transaction Thread # " << t_index <<" Committed " << counter << " updates" << endl;
                    shared_lock->unlock();
                }
            }
        }
    }
    (*threads_results)[t_index] = return_flag;
    // cout << "Finished process internal" <<endl;
    return return_flag;
}


PSQLORMCache::PSQLORMCache()
{
        insert_cache_items_count=0;
        update_cache_items_count=0;
        threads_count = 1;
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


PSQLAbstractORM * PSQLORMCache::fetch (string name,long _identifier)
{
    if (doesExist(name,_identifier)){
        update_cache[name][_identifier]->lock_me(true);
        return update_cache[name][_identifier];
    }
    return NULL;
}

bool PSQLORMCache::doesExist (string name,long _identifier)
{
   if (update_cache.find(name) != update_cache.end()) 
        if (update_cache[name].find(_identifier) != update_cache[name].end()) return true;
    return false;
}


PSQLAbstractORM * PSQLORMCache::add(PSQLAbstractORM * seeder,AbstractDBQuery * psqlQuery,int partition_number)
{
    std::lock_guard<std::mutex> guard(lock);
    long identifier = seeder->getIdentifier(psqlQuery);
    string name = seeder->getORMName();
    PSQLAbstractORM * orm  = fetch(name,identifier);
    if (orm != NULL) return orm;
 
    orm = seeder->clone();
    orm->set_enforced_partition_number(partition_number);
    orm->assignResults(psqlQuery, true);
    // cout << "before lokcing "<< name << endl;
   // orm->lock_me(true);
    // cout << "after lokcing "<< name << endl;
    update_cache[name][identifier] = orm;
    return orm;


}


PSQLAbstractORM * PSQLORMCache::add(string name,PSQLAbstractORM * psqlAbstractORM)
{
    std::lock_guard<std::mutex> guard(lock);
    PSQLAbstractORM * orm = NULL;
    int enforced_cache_index = psqlAbstractORM->get_enforced_partition_number();
    // cout << "enforced_cache_index" << enforced_cache_index << endl;
    if (threads_count < enforced_cache_index)
        threads_count = enforced_cache_index;
    if (enforced_cache_index >=0 )
    {
        for ( int i  = insert_thread_cache.size() ; i < threads_count + 1 ; i++)
            insert_thread_cache.push_back(map <PSQLAbstractORM *,PSQLAbstractORM *> ());
        for ( int i  = update_thread_cache.size() ; i < threads_count + 1 ; i++)
            update_thread_cache.push_back(map <PSQLAbstractORM *,PSQLAbstractORM *> ());
    }
    if (psqlAbstractORM->getIdentifier() == -1 )
    {
        insert_cache[name].push_back(psqlAbstractORM);
        for ( int i  = insert_thread_cache.size() ; i < (insert_cache_items_count%threads_count) +1 ; i++)
            insert_thread_cache.push_back(map <PSQLAbstractORM *,PSQLAbstractORM *> ());
        if (enforced_cache_index == -1)
            insert_thread_cache[insert_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
        else
            insert_thread_cache[enforced_cache_index][psqlAbstractORM]=psqlAbstractORM;
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
                    for ( int i  = update_thread_cache.size() ; i < (update_cache_items_count%threads_count) +1 ; i++)
                        update_thread_cache.push_back(map <PSQLAbstractORM *,PSQLAbstractORM *> ());
                    if (enforced_cache_index == -1)
                        update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
                    else
                    { 
                        update_thread_cache[enforced_cache_index][psqlAbstractORM]=psqlAbstractORM;
                    }
                    update_cache_items_count++;
                }

                lock.unlock();
                psqlAbstractORM->lock_me(true);
                orm->lock_me(true);
                lock.lock();
            }
            else
            {
                psqlAbstractORM->lock_me(true);
                update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
                for ( int i  = update_thread_cache.size() ; i < (update_cache_items_count%threads_count) +1 ; i++)
                    update_thread_cache.push_back(map <PSQLAbstractORM *,PSQLAbstractORM *> ());
                if (enforced_cache_index == -1)
                    update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
                else
                {
                    // cout << "=== Enforcing cache " << enforced_cache_index << endl; 
                    update_thread_cache[enforced_cache_index][psqlAbstractORM]=psqlAbstractORM;
                }
                update_cache_items_count++;
            }
        }
        else
        {
            psqlAbstractORM->lock_me(true);
            // std::ostringstream ss;
            // ss << std::this_thread::get_id() ;
            // printf("assigning new %p for old %p   -   %s\n",psqlAbstractORM,orm,ss.str().c_str());
            update_cache[name][psqlAbstractORM->getIdentifier()]= psqlAbstractORM;
            for ( int i  = update_thread_cache.size() ; i < update_cache_items_count%threads_count +1 ; i++)
                update_thread_cache.push_back(map <PSQLAbstractORM *,PSQLAbstractORM *> ());
            if (enforced_cache_index == -1)
                update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
            else
            {
                // cout << "=== Enforcing cache " << enforced_cache_index << endl; 
                update_thread_cache[enforced_cache_index][psqlAbstractORM]=psqlAbstractORM;
            }
            // update_thread_cache[update_cache_items_count%threads_count][psqlAbstractORM]=psqlAbstractORM;
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


void PSQLORMCache::commit_parallel(string data_source_name, bool transaction, bool orm_transaction)
{
    vector <thread *> threads;
    vector <bool> thread_results (threads_count);
    vector <PSQLConnection *> psqlConnections;
    mutex shared_lock;
    for ( int i  = 0 ; i < threads_count ; i ++)
    {
    
        PSQLConnection * psqlConnection = NULL;
        psqlConnection = psqlController.getPSQLConnection(data_source_name);

        if (transaction && orm_transaction)
        {
            // cout << "#####################333CREATED TRANSACTION" << endl;
            psqlConnection->startTransaction();
        }

        psqlConnections.push_back(psqlConnection);

        thread_results[i] = true;

        thread * t = new thread(commit_parallel_internal,this,i,&shared_lock,psqlConnection,&thread_results, transaction, orm_transaction);
    
        threads.push_back(t);
    }
    for ( int i  = 0 ; i < threads_count ; i ++)
    {
            thread * t = threads[i];
            t->join();
            delete (t);
    }

    if (transaction && orm_transaction)
    {
        bool rollback_flag = false;
        for ( int i  = 0 ; i < threads_count ; i ++)
            if (thread_results[i] == false)
            {
                rollback_flag=true;
                cout << "Rolling Back for thread #" << i <<  endl;
                break;
            }
            rollback_flag = true;
        for ( int i = 0 ; i < threads_count ; i ++)
        {
            if ( rollback_flag )
                psqlConnections[i]->rollbackTransaction();
            else
            { 
                // psqlConnections[i]->rollbackTransaction();
                psqlConnections[i]->commitTransaction();
            }
            psqlController.releaseConnection(data_source_name,psqlConnections[i]);
        }
    } else{
        for ( int i = 0 ; i < threads_count ; i ++)
        {
            psqlController.releaseConnection(data_source_name,psqlConnections[i]);
        }
    }
    ///////////////////////////////////////////////////////////////////

}

void PSQLORMCache::clear_cache(bool clean_updates){
    // cout << "starting cache clear" << endl;
    for (auto orm_cache: insert_cache)
    {
        int counter = 0 ;
        for (auto orm_cache_item:orm_cache.second) 
        {
            // cout << "deleting insert cache #" << counter << endl;
            counter ++;
            delete (orm_cache_item);
        }
        insert_cache[orm_cache.first].clear();
    }
    insert_cache.clear();

    for ( int i = 0 ; i < insert_thread_cache.size() ; i ++)
        insert_thread_cache[i].clear();
    insert_thread_cache.clear();
    insert_cache_items_count =0;

    if (clean_updates)
    {
        // int counter = 0 ;
        for (auto orm_cache: update_cache)
            for (auto orm_cache_item:orm_cache.second) 
            // {
            //     cout << "deleting update cache #" << counter << endl;
            //     counter++;
                    delete (orm_cache_item.second);
            // }

        for (auto orm_cache: update_cache)
            orm_cache.second.clear();

        update_cache.clear();

        for ( int i = 0 ; i < update_thread_cache.size() ; i ++)
            update_thread_cache[i].clear();
        update_thread_cache.clear();
        update_cache_items_count = 0;

    }
}



void PSQLORMCache::commit_sequential (string data_source_name, bool transaction, bool clean_updates)
{
    PSQLConnection * psqlConnection = NULL;
    if (transaction)
    {
        // cout << "Staring Postgresql Transaction" << endl;
        psqlConnection = psqlController.getPSQLConnection(data_source_name);
        psqlConnection->startTransaction();
    }

    long counter = 0;
    for (auto orm_cache: insert_cache)
    {
        for (auto orm_cache_item:orm_cache.second) 
        {
                orm_cache_item->lock_me();
                orm_cache_item->insert(psqlConnection);
                orm_cache_item->unlock_me();
                delete (orm_cache_item);
                counter ++;
                if (counter % 1000 == 0 )
                    cout << "Committed " << counter << " inserts" << endl;
        }
        insert_cache[orm_cache.first].clear();
    }
    insert_cache.clear();
    for ( int i = 0 ; i < insert_thread_cache.size() ; i ++)
        insert_thread_cache[i].clear();
    insert_cache_items_count=0;
    counter = 0;
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            if (orm_cache_item.second->isUpdated())
            {
                orm_cache_item.second->lock_me();
                orm_cache_item.second->update(psqlConnection);
                orm_cache_item.second->unlock_me();
                if (clean_updates) delete (orm_cache_item.second);
                counter ++;
                if (counter % 1000 == 0 )
                    cout << "Committed " << counter << " updates" << endl;
            }
    if(clean_updates)
    {
        for (auto orm_cache: update_cache)
            orm_cache.second.clear();
        update_cache.clear();
        update_cache_items_count=0;
    }
    if (transaction)
    {
        psqlConnection->commitTransaction();
        // psqlConnection->rollbackTransaction();
        psqlController.releaseConnection(data_source_name,psqlConnection);
    }
}

void PSQLORMCache::commit(string data_source_name, bool parallel,bool transaction, bool clean_updates)
{
    cout << "Staring to commit " << endl;


    std::lock_guard<std::mutex> guard(lock);
    if ( parallel ) {
        if (transaction){
            commit_parallel (data_source_name, transaction, false);
            commit_parallel (data_source_name, transaction, true);
        }else commit_parallel (data_source_name, transaction, false);
        clear_cache(clean_updates);
    }
    else commit_sequential(data_source_name, transaction);
    cout << "Exiting commit" << endl;

}
void PSQLORMCache::commit(string data_source_name, string name )
{

}
void PSQLORMCache::commit(string data_source_name, string name,long id)
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
void PSQLORMCache::unlock_current_thread_orms()
{
    std::lock_guard<std::mutex> guard(lock);
    for (auto orm_cache: update_cache)
        for (auto orm_cache_item:orm_cache.second) 
            orm_cache_item.second->unlock_me(true);
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
