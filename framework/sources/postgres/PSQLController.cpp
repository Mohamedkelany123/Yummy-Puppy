#include <PSQLController.h>


bool PSQLController::checkInitialization ()
{
    if(this->psqlConnectionManager == NULL || this->psqlORMCaches == NULL || this->insert_default_values == NULL || this->update_default_values == NULL)
    {
        cout << "Controller not intialized!" << endl;        
        return false;
    }
    return true;
}

/*
    Default Behavior:
    - The controller is initialized with batch mode = true. 
    - In batch mode, the application uses a single cache for all requests.

    When to Set Batch Mode:
    - If you're working with endpoints and need separate caches for each thread, 
    you will need to set the controllers batch mode to false.
*/
thread::id PSQLController::getTeamThreadId()
{
    if (!this->batch_mode)
        return TeamThread::getTeamThreadId();
    else
        return std::thread::id{};
}

//Ensures that a cache is created for the current team thread if it doesn't already exist
void PSQLController::createTeamThreadCache(string data_source_name)
{   
    thread::id team_thread_id = getTeamThreadId();
    if ( ((*psqlORMCaches)[data_source_name])->find(team_thread_id) ==  ((*psqlORMCaches)[data_source_name])->end())
        (*psqlORMCaches)[data_source_name]->operator[](team_thread_id) = new PSQLORMCache();
}


void PSQLController::setBatchMode (bool _batch_mode)
{
    batch_mode = _batch_mode;
}

PSQLController::PSQLController()
{
    this->psqlConnectionManager=NULL;
    this->psqlORMCaches=NULL;
    this->insert_default_values=NULL;
    this->update_default_values=NULL;
    this->batch_mode = true;
}
/*
    Adds a new data source to the controller with the provided connection details.
    Initializes a new cache for the default thread and stores it in the psqlORMCaches map.
*/
bool PSQLController::addDataSource(string data_source_name,string _hostname,int _port,string _database,string _username,string _password)
{
    if(!checkInitialization())
        return false;
    (*psqlORMCaches)[data_source_name] = new map<std::thread::id,PSQLORMCache *> ();
    (*psqlORMCaches)[data_source_name]->operator[](std::thread::id{}) = new PSQLORMCache();
    return psqlConnectionManager->addDataSource(data_source_name,_hostname,_port,_database,_username,_password);
}
//Checks if the data source is initialized
bool PSQLController::isDataSource(string data_source_name)
{
    return psqlConnectionManager->isDataSource(data_source_name);
}  


/*
    Returns the default data source if an empty string is passed.
    If a non-empty data source name is provided, it checks whether the data source is available.
*/
string PSQLController::checkDefaultDatasource(string data_source_name){
    if(!checkInitialization())
        return "";
    if (data_source_name == "") return psqlConnectionManager->getDefaultDatasource();
    else if (psqlORMCaches->find(data_source_name) == psqlORMCaches->end())
        throw std::runtime_error("ERROR :: Invalid Data Source Name");

    return data_source_name;
}
//Returns a psqlconnection to the database from the psqlConnectionManagers connection pool.
PSQLConnection * PSQLController::getPSQLConnection(string data_source_name)
{
    if(!checkInitialization())
        return nullptr;
    return psqlConnectionManager->getPSQLConnection(data_source_name);
}

//Releases the psqlconnection back to the psqlConnectionManagers connection pool.
bool PSQLController::releaseConnection (string data_source_name,PSQLConnection * psqlConnection)
{
    if(!checkInitialization())
        return false;
    return psqlConnectionManager->releaseConnection(data_source_name,psqlConnection);
}

//Adds an entity to the ORM cache
PSQLAbstractORM * PSQLController::addToORMCache(string name,PSQLAbstractORM * psqlAbstractORM, string data_source_name)
{
    if(!checkInitialization())
        return nullptr;
    data_source_name = checkDefaultDatasource(data_source_name);
    createTeamThreadCache(data_source_name);
    return (*psqlORMCaches)[data_source_name]->operator[](getTeamThreadId())->add(name,psqlAbstractORM);
}

PSQLAbstractORM * PSQLController::addToORMCache(PSQLAbstractORM * seeder, AbstractDBQuery * _psqlQuery, int _partition_number, string data_source_name )
{
    if(!checkInitialization())
        return nullptr;
    data_source_name = checkDefaultDatasource(data_source_name);
    createTeamThreadCache(data_source_name);
    return (*psqlORMCaches)[data_source_name]->operator[](getTeamThreadId())->add(seeder,_psqlQuery,_partition_number);
}

// Commits changes for all data sources and their associated sub-caches for all team threads. 
void PSQLController::ORMCommitAll(bool parallel,bool transaction,bool clean_updates)
{
    if(!checkInitialization())
        return;
    for (auto cache_group : *psqlORMCaches){
        ORMCommit(parallel, transaction, clean_updates, cache_group.first);
    }
}
void PSQLController::ORMCommit(bool parallel,bool transaction,bool clean_updates, string data_source_name)
{
    if(!checkInitialization())
        return;
    data_source_name = checkDefaultDatasource(data_source_name);
    for (auto cache : *((*psqlORMCaches)[data_source_name]))
        cache.second->commit(data_source_name, parallel,transaction,clean_updates);
}

map<string, vector<long>> PSQLController::ORMCommit_me(bool transaction,bool clean_updates)
{   
    map<string, vector<long>> inserted_ids;
    for (auto cache_group : *psqlORMCaches)
        inserted_ids = ORMCommit_me(cache_group.first, transaction, clean_updates);
    
    return inserted_ids;
}

map<string, vector<long>> PSQLController::ORMCommit_me(string data_source_name, bool transaction,bool clean_updates)
{
    map<string, vector<long>> inserted_ids;
    if(!checkInitialization())
        return inserted_ids;
    std::thread::id team_id = getTeamThreadId();

    data_source_name = checkDefaultDatasource(data_source_name);
    if ( ((*psqlORMCaches)[data_source_name])->find(team_id) !=  ((*psqlORMCaches)[data_source_name])->end()){
        PSQLORMCache * cache = ((*psqlORMCaches)[data_source_name])->operator[](team_id);
        
        //Unlock all orms as they are locked from PSQLORMCache::add() function.
        cache->unlock_current_thread_orms();
        inserted_ids = cache->commit(data_source_name, false,transaction,clean_updates);

        if(clean_updates){
            ((*psqlORMCaches)[data_source_name])->erase(team_id);
            delete cache;
        }
    }

    return inserted_ids;
}


void PSQLController::ORMFlush()
{
    if(!checkInitialization())
        return;
    for (auto cache_group : *psqlORMCaches)
        ORMFlush(cache_group.first);


}
void PSQLController::ORMFlush(string data_source_name)
{
    if(!checkInitialization())
        return;
    data_source_name = checkDefaultDatasource(data_source_name);
    for (auto cache : *((*psqlORMCaches)[data_source_name]))
        cache.second->flush();

}

void PSQLController::ORMFlush_me()
{
    for (auto cache_group : *psqlORMCaches)
        ORMFlush_me(cache_group.first);

}

void PSQLController::ORMFlush_me(string data_source_name)
{
    if(!checkInitialization())
        return;
    std::thread::id team_id = getTeamThreadId();

    data_source_name = checkDefaultDatasource(data_source_name);
   
    if ( ((*psqlORMCaches)[data_source_name])->find(team_id) !=  ((*psqlORMCaches)[data_source_name])->end())
            ((*psqlORMCaches)[data_source_name])->operator[](team_id)->flush();

}

int PSQLController::getDataSourceConnectionCount(string data_source_name)
{
    if(!checkInitialization())
        return -1;
    return psqlConnectionManager->getConnectionCount(data_source_name);

}
void PSQLController::setAllORMCacheThreads (int _threads_count)
{   
    if(!checkInitialization())
        return;
    for (auto cache : *psqlORMCaches){
        setORMCacheThreads(_threads_count, cache.first);
    }
}

void PSQLController::setORMCacheThreads (int _threads_count, string data_source_name)
{
    if(!checkInitialization())
        return;
    data_source_name = checkDefaultDatasource(data_source_name);
    for (auto  cache : *((*psqlORMCaches)[data_source_name]))
        cache.second->set_threads_count(_threads_count);
}


void PSQLController::unlock_all_current_thread_orms()
{
    if(!checkInitialization())
        return;
    for (auto cache : *psqlORMCaches){
        unlock_current_thread_orms(cache.first);
    }
}

void PSQLController::unlock_current_thread_orms(string data_source_name)
{
    if(!checkInitialization())
    return;
    data_source_name = checkDefaultDatasource(data_source_name);
    for (auto  cache : *((*psqlORMCaches)[data_source_name]))
        cache.second->unlock_current_thread_orms();
}

void PSQLController::addDefault(string name,string value, bool is_insert, bool is_func)
{
    if(!checkInitialization())
        return;
    pair<string, bool> p; 
    p.first = value;
    p.second = is_func;
    if ( is_insert )
        (*insert_default_values)[name]= p;
    else (*update_default_values)[name]= p;
}

map <string,pair<string,bool>> * PSQLController::getUpdateDefaultValues()
{
    if(!checkInitialization())
        return nullptr;
    return update_default_values;
}
map <string,pair<string,bool>> * PSQLController::getInsertDefaultValues()
{
    if(!checkInitialization())
        return nullptr;
    return insert_default_values;
}

int PSQLController::getCacheCounter (string _data_source_name)
{
    if(!checkInitialization())
        return -1;
    _data_source_name = checkDefaultDatasource(_data_source_name);
    int cache_counter = 0;
    for (auto  cache : *((*psqlORMCaches)[_data_source_name]))
        cache_counter += cache.second->cache_counter;
    return cache_counter;
}


PSQLController::~PSQLController()
{
    if(this->psqlORMCaches != NULL)
        for (auto cache_group : *psqlORMCaches) {
            for ( auto cache : *(cache_group.second))
                delete (cache.second);
            delete (cache_group.second);
        }
}

PSQLConnectionManager * PSQLController::get_psqlConnectionManager(){return psqlConnectionManager;}
map <string, map<std::thread::id,PSQLORMCache *> *> * PSQLController::get_psqlORMCaches(){return psqlORMCaches;}


 //************************************************

PSQLControllerMaster::PSQLControllerMaster():PSQLController()
{

    psqlConnectionManager = new PSQLConnectionManager();
    psqlORMCaches = new map <string, map<std::thread::id,PSQLORMCache *> *> ();
    insert_default_values = new map <string,pair<string,bool>>();
    update_default_values = new map <string,pair<string,bool>>();
    printf ("PSQL Controller Initialized\n");
    cout << "START MASTER CONSTRUCTOR:" << endl;
    cout << "psqlConnectionManager:" << psqlConnectionManager << endl;
    cout << "psqlORMCaches:" << psqlORMCaches << endl;
    cout << "update_default_values:" << update_default_values << endl;
    cout << "insert_default_values:" << insert_default_values << endl;
    cout << "END MASTER CONSTRUCTOR:\n\n";
}
void PSQLControllerMaster::initialize(PSQLController * psqlControllerMaster)
{
    return;
}
PSQLControllerMaster::~PSQLControllerMaster()
{   
    if(this->psqlConnectionManager != NULL)
        delete (psqlConnectionManager);
    
    if(this->insert_default_values != NULL)
        delete (insert_default_values);

    if(this->update_default_values != NULL)
        delete (update_default_values);
}

 //************************************************

PSQLControllerSlave::PSQLControllerSlave():PSQLController()
{
    cout << "DEFINED SAVE\n\n";
}
void PSQLControllerSlave::initialize(PSQLController * psqlControllerMaster)
{
    this->psqlConnectionManager=psqlControllerMaster->get_psqlConnectionManager();
    this->psqlORMCaches=psqlControllerMaster->get_psqlORMCaches();
    this->insert_default_values=psqlControllerMaster->getInsertDefaultValues();
    this->update_default_values=psqlControllerMaster->getUpdateDefaultValues();

    cout << "START SLAVE INITIALIZE CONSTRUCTOR:" << endl;
    cout << "psqlConnectionManager:" << this->psqlConnectionManager << endl;
    cout << "psqlORMCaches:" << this->psqlORMCaches << endl;
    cout << "update_default_values:" << this->update_default_values << endl;
    cout << "insert_default_values:" << this->insert_default_values << endl;
    cout << "END SLAVE INITIALIZE CONSTRUCTOR:\n\n";
}
PSQLControllerSlave::~PSQLControllerSlave()
{
}

#ifdef SHARED_LIBRARY_FLAG
PSQLControllerSlave psqlController; 
#else
PSQLControllerMaster psqlController;
#endif



