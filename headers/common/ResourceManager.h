#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <common.h>

template <typename T>
class ResourceManager 
{
    protected:
        map <uint64_t,T *> available_resources;
        map <uint64_t,T *> reserved_resources;
        std::function<T *(ResourceManager<T> * me)> lambda;
        std::mutex lock;
    public:
        // ResourceManager ()
        // {
        //     lambda = [](ResourceManager <T> * me)-> T* {return NULL;} ;
        // }
        ResourceManager(std::function<T * (ResourceManager<T> * me)> _lambda = [](ResourceManager <T> * me)-> T* {return NULL;})
        {
            lambda = _lambda;
        }
        T * getResource(){
            std::lock_guard<std::mutex> guard(lock);
            T * resource  =  NULL;
            if (available_resources.empty())
            {
                cout << "No resource available, creating one ..." << endl;
                resource  =  lambda(this);
                if ( resource != NULL)
                    reserved_resources[(uint64_t)resource] = resource;
            }
            else
            {
                typename map<uint64_t,T* >::iterator victim = available_resources.begin();
                reserved_resources[victim->first] = victim->second;
                resource = victim->second;
                available_resources.erase(victim->first); // erase will fuck up the victim on mac, logically it should not :)
                cout << "Found one in the pool "<< endl;
            }
            return resource;
        }
        bool releaseResource (T * _resource)
        {
            std::lock_guard<std::mutex> guard(lock);
            if (reserved_resources.find((uint64_t)_resource) != reserved_resources.end())
            {
                available_resources[(uint64_t)_resource] = _resource;
                reserved_resources.erase((uint64_t)_resource);
                return true;
            }
            return false;
        }
        int getTotalResourceCount()
        {
            return available_resources.size()+reserved_resources.size();
        }
        int getAvailableResourceCount()
        {
            return available_resources.size();
        }
        int getReservedResourceCount()
        {
            return reserved_resources.size();
        }
        virtual ~ResourceManager()
        {
            cout << "Destructing resouces" << endl;
            std::lock_guard<std::mutex> guard(lock);
            for (auto available_resource: available_resources) 
                delete (available_resource.second);

            for (auto reserved_resource: reserved_resources) 
                delete (reserved_resource.second);
        }
};

#endif