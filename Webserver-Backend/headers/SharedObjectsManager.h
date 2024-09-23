#ifndef _SharedObjectsManager_H
#define _SharedObjectsManager_H

#include "includes.h"
// template<class T>
// T * create_object_routine();

template <typename T> using create_object_routine = T* (*)(PSQLControllerMaster * psqlControllerMaster);

template <typename T> 
class SharedObjectsManager{
    protected:
        map <string,T *> gobj; // A vector to store the cloner objects
        map <string,void *> dsos; // A vector to store all DSO objects

    public:
        SharedObjectsManager(){} 
	    T * load(string name, string funcName="create_object" ){ // Load object form DSO
            T * obj  = gobj[name]; // Fetch object based on name
            if ( obj != NULL ) return obj;  // If found return it
            // Else load the DSO and fetch the object through external launcher function
                string lib_name = name; // Compose DSO name
                void * dso=dlopen (lib_name.c_str(),RTLD_LAZY);
                
                if ( dso ){ // If dso is not NULL then loaded successfully
                
                    create_object_routine<T> my_func = (create_object_routine<T> ) dlsym (dso,funcName.c_str()); // Loading external function from DSO

                    if ( my_func ){ // If function loaded successfully
                    
                        gobj[name] = my_func(psqlController);    // Store function in gobj vector
                        dsos[name] = dso;          // Store DSO handler into dsos vectors
                        obj  = gobj[name];         // Fetch the external function into obj to be returned
                    } // Cannot load the external function from the DSO
                    else{ 
                        string s = "Error fetching external function " + name  + ": " + dlerror()  + ", Log level: LOG_ERROR" ;
                        LOG_ERRORS(s);
                        s= "";
                    }
                }   // Unable to load then print error message with dlerror()
                else{
                    string s = "Error loading dynamic object " + name  + ": " + dlerror()  + ", Log level: LOG_ERROR" ;
                    LOG_ERRORS(s);
                    s = "";
                }
        
            return obj; // Return required object
        }
        ~SharedObjectsManager(){
        // An iterator loop with a lambda function to delete all objects
            for_each (gobj.begin(),gobj.end(),[](const std::pair<string,T *>& it) -> bool {
                        T * obj = std::get<1>(it);
                delete (obj);
                        return true;
                });
            // An iteration loop with a lambda function to close all the dlls
            for_each (dsos.begin(),dsos.end(),[](const std::pair<string,void *>& it) -> bool {
                void * dso = std::get<1>(it);
                dlclose(dso);
                return true;
            });
        }
};

#endif