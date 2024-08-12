
/**
 * File: TensorRTGlobalService.cpp 
 * Date: Mar 2022
 * Company: Blnk Consumer Finance
 * Author: Ghada Gamal
 */

#include "SharedObjectsManager.h"

 SharedObjectsManager::SharedObjectsManager(){} // Constructor

  HTTPService * SharedObjectsManager::load(string name) { // Load object form DSO
    HTTPService * obj  = gobj[name]; // Fetch object based on name
    if ( obj != NULL ) return obj;  // If found return it
    // Else load the DSO and fetch the object through external launcher function
        string lib_name = name; // Compose DSO name
        void * dso=dlopen (lib_name.c_str(),RTLD_LAZY);
        
        if ( dso ){ // If dso is not NULL then loaded successfully
         
            create_object_routine * my_func = (create_object_routine *) dlsym (dso,"create_object"); // Loading external function from DSO

            if ( my_func ){ // If function loaded successfully
             
                gobj[name] = my_func();    // Store function in gobj vector
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

  Middleware * SharedObjectsManager::loadMiddleware(string name) { // Load object form DSO
    Middleware * obj  = middles[name]; // Fetch object based on name
    if ( obj != NULL ) return obj;  // If found return it
    // Else load the DSO and fetch the object through external launcher function
        string lib_name = name; // Compose DSO name
        void * dso=dlopen (lib_name.c_str(),RTLD_LAZY);
        
        if ( dso ){ // If dso is not NULL then loaded successfully
         
            create_middleware_routine * my_func = (create_middleware_routine *) dlsym (dso,"create_object"); // Loading external function from DSO

            if ( my_func ){ // If function loaded successfully
             
                middles[name] = my_func();    // Store function in gobj vector
                dsos[name] = dso;          // Store DSO handler into dsos vectors
                obj  = middles[name];         // Fetch the external function into obj to be returned
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

SharedObjectsManager::~SharedObjectsManager()
{
   // An iterator loop with a lambda function to delete all objects
	for_each (gobj.begin(),gobj.end(),[](const std::pair<string,HTTPService *>& it) -> bool {
                HTTPService * obj = std::get<1>(it);
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